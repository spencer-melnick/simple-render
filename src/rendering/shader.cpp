#include "shader.hpp"

#include <spdlog/spdlog.h>

#include "context.hpp"
#include "util/simplefile.hpp"

namespace Rendering
{
    Shader::Shader(const std::string_view& sourcePath)
    {
        Util::SimpleFile sourceFile(sourcePath);
        const uint32_t* shaderCode = sourceFile.getContentsRaw<uint32_t>();

        vk::ShaderModuleCreateInfo createInfo;
        createInfo.codeSize = sourceFile.getContents().size();
        createInfo.pCode = shaderCode;

        spdlog::info("Creating shader module \"{}\"", sourcePath);
        m_shaderModule = Context::getVulkanDevice().createShaderModuleUnique(createInfo);
    }

    Shader::~Shader()
    {
        spdlog::info("Destroying shader module");
    }
}
