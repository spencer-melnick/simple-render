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

            vk::UniqueSwapchainKHR m_swapchain;
            std::vector<vk::Image> m_swapchainImages;
    };
}
