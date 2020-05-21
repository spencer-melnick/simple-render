#include "commandbuffer.hpp"

#include <spdlog/spdlog.h>

#include "context.hpp"

namespace Rendering
{
    CommandBuffer::CommandBuffer(CommandBuffer::Type type)
    {
        vk::CommandBufferAllocateInfo allocateInfo;
        allocateInfo.commandBufferCount = 1;
        allocateInfo.commandPool = Context::getCommandPool();
        
        switch (type)
        {
            case CommandBuffer::Type::Primary:
                allocateInfo.level = vk::CommandBufferLevel::ePrimary;
                break;

            case CommandBuffer::Type::Secondary:
                allocateInfo.level = vk::CommandBufferLevel::eSecondary;
        }

        m_commandBuffer = std::move(Context::getVulkanDevice().allocateCommandBuffersUnique(allocateInfo).front());
    }
}
