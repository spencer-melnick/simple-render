#pragma once

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

            Swapchain(Device& device, Window& window);
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
            // Creates framebuffers for each swapchain image that support
            // the image types required by the render pass, and stores
            // a reference (in case the swapchain needs to be recreated)
            void attachRenderPass(Device& device, Pass& pass);

        private:
            // Initialization steps
            void createVulkanSwapchain(Device& device, Window& window);
            void aquireSwapchainImages(Device& device);

            vk::SurfaceFormatKHR m_surfaceFormat;
            vk::Extent2D m_swapchainExtents;
            vk::UniqueSwapchainKHR m_swapchain;
            std::vector<Image> m_swapchainImages;
            std::optional<std::reference_wrapper<Pass>> m_associatedPass;

            // TODO: Move swapchain out of context?
            // possibly into window
    };
}
