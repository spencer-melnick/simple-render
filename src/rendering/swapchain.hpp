#pragma once

#include <vector>

#include <vulkan/vulkan.hpp>

namespace Rendering
{
    extern vk::PresentModeKHR defaultPresentMode;

    class Device;
    class Window;

    class Swapchain
    {
        public:
            struct Image
            {
                Image(vk::Image image) :
                    image(image), imageView(nullptr)
                {};

                vk::Image image;
                vk::UniqueImageView imageView;
            };

            Swapchain(Device& device, Window& window);
            ~Swapchain();

            const vk::SwapchainKHR& getSwapchain() const {
                return *m_swapchain;
            }
            const auto& getSwapchainImages() const {
                return m_swapchainImages;
            }

        private:
            // Initialization steps
            void createVulkanSwapchain(Device& device, Window& window);
            void aquireSwapchainImages(Device& device);

            vk::SurfaceFormatKHR m_surfaceFormat;
            vk::UniqueSwapchainKHR m_swapchain;
            std::vector<Image> m_swapchainImages;

            // TODO: Move swapchain out of context?
            // possibly into window
    };
}
