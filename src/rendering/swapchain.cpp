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

    Swapchain::Swapchain(Device& device, Window& window)
    {
        createVulkanSwapchain(device, window);
        aquireSwapchainImages(device);
    }

    Swapchain::~Swapchain()
    {
        spdlog::info("Destroying swapchain");
    }

    const Swapchain::Image& Swapchain::getNextImage() const
    {
        const Swapchain::Image& currentImage = m_swapchainImages[m_currentFrame];

        // Context::getVulkanDevice().acquireNextImageKHR(*m_swapchain,
            // std::numeric_limits<uint64_t>::max, *currentImage.imageAvailable,
            // nullptr);

        return currentImage;
    }


    void Swapchain::attachRenderPass(Device& device, Pass& pass)
    {
        spdlog::info("Attaching render pass to swapchain");

        if (m_associatedPass.has_value())
        {
            spdlog::warn("Swapchain already has framebuffers associated with a render pass");
            spdlog::warn("The previously associated render pass may not function properly");
        }

        m_associatedPass = pass;

        spdlog::info("Creating {} framebuffers", m_swapchainImages.size());
        for (auto& i : m_swapchainImages)
        {
            vk::FramebufferCreateInfo createInfo;
            createInfo.attachmentCount = 1;
            createInfo.pAttachments = &i.imageView.get();
            createInfo.width = m_swapchainExtents.width;
            createInfo.height = m_swapchainExtents.height;
            createInfo.layers = 1;
            createInfo.renderPass = pass.getRenderPass();

            i.framebuffer = device.getVulkanDevice().createFramebufferUnique(createInfo);
        }
    }


    void Swapchain::createVulkanSwapchain(Device& device, Window& window)
    {
        // Grab the first surface format
        m_surfaceFormat = device.getProperties().getSurfaceFormats()[0];
        auto presentMode = defaultPresentMode;

        // Find surface exent limits
        auto surfaceCapabilties =
            device.getProperties().getPhysicalDevice().getSurfaceCapabilitiesKHR(window.getSurface());

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

        if (device.getProperties().getGraphicsQueue() == device.getProperties().getPresentationQueue())
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
                device.getProperties().getGraphicsQueue(),
                device.getProperties().getPresentationQueue()
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
            m_swapchain = device.getVulkanDevice().createSwapchainKHRUnique(createInfo);
        }
        catch (const std::exception& exception)
        {
            spdlog::error("Failed to create Vulkan swapchain: {}", exception.what());
            throw exception;
        }
    }

    void Swapchain::aquireSwapchainImages(Device& device)
    {
        spdlog::info("Aquiring swapchain images");
        auto swapchainImages = device.getVulkanDevice().getSwapchainImagesKHR(m_swapchain.get());

        spdlog::info("Creating swapchain image views");
        for (auto& i : swapchainImages)
        {
            auto& imageWithView = m_swapchainImages.emplace_back(i);
            
            vk::ImageViewCreateInfo createInfo;
            createInfo.image = i;
            createInfo.viewType = vk::ImageViewType::e2D;
            createInfo.format = m_surfaceFormat.format;
            createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.layerCount = 1;
            createInfo.subresourceRange.levelCount = 1;

            imageWithView.imageView = device.getVulkanDevice().createImageViewUnique(createInfo);
        }
        spdlog::info("Aquired {} swapchain images", m_swapchainImages.size());
    }

    void Swapchain::createFrameData()
    {

    }
}
