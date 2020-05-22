#pragma once

#include <array>
#include <vector>
#include <optional>
#include <functional>

#include <vulkan/vulkan.hpp>

namespace Rendering
{
    extern vk::PresentModeKHR defaultPresentMode;

    class Device;
    class Window;
    class Pass;

    class Swapchain
    {
        friend class Pass;

        public:
            struct Image
            {
                Image(vk::Image image) :
                    image(image), imageView(nullptr), framebuffer(nullptr)
                {};

                vk::Image image;
                vk::UniqueImageView imageView;
                vk::UniqueFramebuffer framebuffer;
            };

            Swapchain(Window& window, Pass& pass);
            ~Swapchain();


            auto getSurfaceFormat() const {
                return m_surfaceFormat;
            }
            auto getSwapchainExtents() const {
                return m_swapchainExtents;
            }
            const vk::SwapchainKHR& getSwapchain() const {
                return *m_swapchain;
            }
            const auto& getSwapchainImages() const {
                return m_swapchainImages;
            }

        protected:

        private:
            // Initialization steps
            void createVulkanSwapchain(Window& window);
            void aquireSwapchainImages();
            void createFramebuffers();

            vk::SurfaceFormatKHR m_surfaceFormat;
            vk::Extent2D m_swapchainExtents;
            vk::UniqueSwapchainKHR m_swapchain;
            std::vector<Image> m_swapchainImages;
            

            // Store reference to render pass in case we need to
            // recreate the swapchain
            Pass& m_renderPass; 
    };
}
