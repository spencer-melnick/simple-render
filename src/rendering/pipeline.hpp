#pragma once

#include <vulkan/vulkan.hpp>

#include "shader.hpp"
#include "pass.hpp"

namespace Rendering
{
    class Pipeline
    {
        public:
            Pipeline(Shader& vertexShader, Shader& fragmentShader, Pass& pass);
            ~Pipeline();

            const vk::Pipeline& getPipeline() const {
                return *m_pipeline;
            }

        private:
            vk::UniquePipelineLayout m_pipelineLayout;
            vk::UniquePipeline m_pipeline;
    };
}
