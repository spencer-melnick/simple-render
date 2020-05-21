#pragma once

#include <vulkan/vulkan.hpp>

namespace Rendering
{
    class CommandBuffer
    {
        public:
            enum class Type
            {
                Primary,
                Secondary
            };

            CommandBuffer(Type type);

            const vk::CommandBuffer& getCommandBuffer() const {
                return *m_commandBuffer;
            }

        private:
            vk::UniqueCommandBuffer m_commandBuffer;
    };
}
