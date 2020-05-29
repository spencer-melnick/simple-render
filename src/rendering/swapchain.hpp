#pragma once

#include <array>
#include <vector>
#include <optional>
#include <functional>
#include <tuple>

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
                Image(vk::Image image, uint32_t index) :
                    image(image), imageView(nullptr), framebuffer(nullptr),
                    index(index)
                {};

                vk::Image image;
                vk::UniqueImageView imageView;
                vk::UniqueFramebuffer framebuffer;
                uint32_t index;
            };

            enum class ImageAquisitionError
            {
                NoError,
                ShouldRecreateSwapchain
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

            using ImageAquisitionResult =
                std::tuple<ImageAquisitionError, std::optional<std::reference_wrapper<const Image>>>;

            // Returns the next image to be presented to in the swapchain
            // and signals semaphore when it is available to be used on
            // success. If unsucessful, the reference is unset and the
            // result type will tell why. Can fail on swapchain being
            // out of date, in which case the swapchain will need to be
            // recreated.
            ImageAquisitionResult getNextImage(const vk::Semaphore& semaphore);

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
