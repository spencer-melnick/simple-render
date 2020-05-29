#pragma once

#include <vulkan/vulkan.hpp>

namespace Rendering
{
    // A collection of helper functions that can be used with a 
    // Vulkan command buffer
    namespace Commands
    {
        // Sets the scissor and viewport for the command buffer to the
        // specified width and height. Must be used inside of a render
        // pass.
        void setScissorAndViewport(const vk::CommandBuffer& commandBuffer,
            uint32_t width, uint32_t height);
    }
}
