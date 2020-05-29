#pragma once

#include <vulkan/vulkan.hpp>

namespace Rendering
{
    // Everything that we need to render that isn't one-per application
    class Renderer
    {
        public:
            Renderer();
            ~Renderer();


            // Main functionality

            // Renders a frame using the provided objects
            void render(class Swapchain& swapchain, const class Pass& pass,
                const class Pipeline& pipeline);

            // Waits until all of the primary command buffers in the frame
            // data are inactive.
            void waitForCommandsFree();

        private:
            // Based on Intel's experiments, it's best to keep frame data
            // for only two frames, i.e. simple double buffering
            static const size_t FrameCount = 2;

            // Data that can't be reused between frames. This includes
            // the primary command buffer that needs to reference a swapchain
            // image (which changes frame to frame) and synchronization
            // structures
            struct FrameData
            {
                FrameData() :
                    imageAvailable(nullptr), renderFinished(nullptr),
                    fence(nullptr)
                {};

                vk::UniqueSemaphore imageAvailable;
                vk::UniqueSemaphore renderFinished;
                vk::UniqueFence fence;
                vk::UniqueCommandBuffer commandBuffer;
            };

            // Initialization steps
            void createCommandPools();
            void createFrameData();

            // Member variables
            vk::UniqueCommandPool m_primaryCommandPool;

            // Frame specific
            std::array<FrameData, FrameCount> m_frameData;
            size_t m_currentFrame;
    };
}
