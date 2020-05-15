#pragma once

#include <vulkan/vulkan.hpp>

#include "shader.hpp"

namespace Rendering
{
    class Pipeline
    {
        public:
            Pipeline(Shader& vertexShader, Shader& fragmentShader);
            ~Pipeline();

        private:
            vk::UniquePipelineLayout m_pipelineLayout;
    };
}
