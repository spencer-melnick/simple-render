#include "swapchain.hpp"

#include <algorithm>
#include <limits>

#include <spdlog/spdlog.h>
#include <SDL2/SDL_vulkan.h>

#include "context.hpp"
#include "device.hpp"
#include "window.hpp"
#include "pass.hpp"

namespace Rendering
{
    vk::PresentModeKHR defaultPresentMode = vk::PresentModeKHR::eFifo;

    Swapchain::Swapchain(Window& window, Pass& pass) :
        m_renderPass(pass)
    {
        createVulkanSwapchain(window);
        aquireSwapchainImages();
        createFramebuffers();
    }

    Swapchain::~Swapchain()
    {
        spdlog::info("Destroying swapchain");
    }



    const Swapchain::Image& Swapchain::getNextImage(const vk::Semaphore& semaphore)
    {
        // Aquire the next swapchain image and signal semaphore
        // when it's ready
        size_t swapchainImageIndex = static_cast<size_t>(
            Rendering::Context::getVulkanDevice().acquireNextImageKHR(
                *m_swapchain,
                (std::numeric_limits<uint64_t>::max)(),
                semaphore,
                nullptr
            ).value);

        if (swapchainImageIndex >= m_swapchainImages.size())
        {
            throw std::exception("Next available swapchain image is out of range");
        }

        return m_swapchainImages[swapchainImageIndex];
    }


    // Initialization steps

    void Swapchain::createVulkanSwapchain(Window& window)
    {
        // Grab the first surface format
        m_surfaceFormat = Context::get().getDevice().getSurfaceFormat();
        auto presentMode = defaultPresentMode;

        // Find surface exent limits
        auto surfaceCapabilties =
            Context::get().getDevice().getProperties().getPhysicalDevice().getSurfaceCapabilitiesKHR(window.getSurface());

        // Query the actual resolution of the Vulkan window
        int swapchainWidth, swapchainHeight;
        SDL_Vulkan_GetDrawableSize(window.getSdlWindow(), &swapchainWidth, &swapchainHeight);

        // Limit extents by surface capabilities
        m_swapchainExtents.width = std::clamp(static_cast<uint32_t>(swapchainWidth),
            surfaceCapabilties.maxImageExtent.width, surfaceCapabilties.minImageExtent.width);
        m_swapchainExtents.height = std::clamp(static_cast<uint32_t>(swapchainHeight),
            surfaceCapabilties.maxImageExtent.height, surfaceCapabilties.minImageExtent.height);

        // Set image count to the minimum required plus one, unless limited
        uint32_t imageCount = surfaceCapabilties.minImageCount + 1;
        if (imageCount > surfaceCapabilties.maxImageCount && surfaceCapabilties.maxImageCount != 0)
        {
            imageCount = surfaceCapabilties.minImageCount;
        }

        // Create swapchain information
        vk::SwapchainCreateInfoKHR createInfo;
        createInfo.surface = window.getSurface();
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = m_surfaceFormat.format;
        createInfo.imageColorSpace = m_surfaceFormat.colorSpace;
        createInfo.imageExtent = m_swapchainExtents;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

        if (Context::get().getDevice().getProperties().getGraphicsQueue() ==
            Context::get().getDevice().getProperties().getPresentationQueue())
        {
            // If presentation and graphics queues use the same family
            // then the swapchain can be used in exclusive mode

            createInfo.imageSharingMode = vk::SharingMode::eExclusive;
        }
        else
        {
            // Otherwise it must be shared in concurrent mode
            // (for basic usage)
            std::vector<uint32_t> queueIndices = {
                Context::get().getDevice().getProperties().getGraphicsQueue(),
                Context::get().getDevice().getProperties().getPresentationQueue()
            };

            createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
            createInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueIndices.size());
            createInfo.pQueueFamilyIndices = queueIndices.data();
        }

        // Some additional settings to control presentation
        createInfo.preTransform = surfaceCapabilties.currentTransform;
        createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
        createInfo.clipped = true;
        createInfo.presentMode = presentMode;
        createInfo.oldSwapchain = nullptr;

        // Create the Vulkan handle and log additional information on error
        spdlog::info("Creating Vulkan swapchain");
        try
        {
            m_swapchain = Context::getVulkanDevice().createSwapchainKHRUnique(createInfo);
        }
        catch (const std::exception& exception)
        {
            spdlog::error("Failed to create Vulkan swapchain: {}", exception.what());
            throw exception;
        }
    }

    void Swapchain::aquireSwapchainImages()
    {
        spdlog::info("Aquiring swapchain images");
        auto swapchainImages = Context::getVulkanDevice().getSwapchainImagesKHR(m_swapchain.get());

        spdlog::info("Creating swapchain image views");
        for (size_t i = 0; i < swapchainImages.size(); i++)
        {
            vk::Image& image = swapchainImages[i];
            auto& imageWithView = m_swapchainImages.emplace_back(image, static_cast<uint32_t>(i));
            
            vk::ImageViewCreateInfo createInfo;
            createInfo.image = image;
            createInfo.viewType = vk::ImageViewType::e2D;
            createInfo.format = m_surfaceFormat.format;
            createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.layerCount = 1;
            createInfo.subresourceRange.levelCount = 1;

            imageWithView.imageView = Context::getVulkanDevice().createImageViewUnique(createInfo);
        }
        spdlog::info("Aquired {} swapchain images", m_swapchainImages.size());
    }

    void Swapchain::createFramebuffers()
    {
        spdlog::info("Creating {} framebuffers", m_swapchainImages.size());
        for (auto& i : m_swapchainImages)
        {
            vk::FramebufferCreateInfo createInfo;
            createInfo.attachmentCount = 1;
            createInfo.pAttachments = &i.imageView.get();
            createInfo.width = m_swapchainExtents.width;
            createInfo.height = m_swapchainExtents.height;
            createInfo.layers = 1;
            createInfo.renderPass = m_renderPass.getRenderPass();

            i.framebuffer = Context::getVulkanDevice().createFramebufferUnique(createInfo);
        }
    }
}
