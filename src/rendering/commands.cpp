#include "commands.hpp"

#include <vulkan/vulkan.hpp>

namespace Rendering
{
    namespace Commands
    {
        void setScissorAndViewport(const vk::CommandBuffer& commandBuffer,
            uint32_t width, uint32_t height)
        {
            commandBuffer.setViewport(0, {vk::Viewport{
                0, 0,
                static_cast<float>(width), static_cast<float>(height),
                0.0f, 1.0f
            }});
            commandBuffer.setScissor(0, {vk::Rect2D{
                {0, 0},
                {width, height}
            }});
        }
    }
}
