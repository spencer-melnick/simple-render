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

            const auto& getShaderModule() const {
                return m_shaderModule;
            }

        private:
            vk::UniqueShaderModule m_shaderModule;
    };
}
