#pragma once

#include <vector>

#include <vulkan/vulkan.hpp>

namespace Rendering
{
    class Swapchain;

    // Does nothing right now!
    class Subpass
    {

    };

    class Pass
    {
        public:
            Pass();
            ~Pass();

            const vk::RenderPass& getRenderPass() const {
                return *m_renderPass;
            }

        private:
            vk::UniqueRenderPass m_renderPass;
    };
}
