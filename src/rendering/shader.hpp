#pragma once

#include <string_view>

#include <vulkan/vulkan.hpp>

namespace Rendering
{
    class Shader
    {
        public:
            Shader(const std::string_view& sourcePath);
            ~Shader();

            vk::ShaderModule& getShaderModule() {
                return m_shaderModule.get();
            }

        private:
            vk::UniqueShaderModule m_shaderModule;
    };
} 
