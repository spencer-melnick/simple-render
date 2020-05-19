#include "pipeline.hpp"

#include <vector>

#include <spdlog/spdlog.h>

#include "context.hpp"

namespace Rendering
{
    // TODO: Add a lot configuration here!
    // Multiple pipelines for different things would be useful
    // This is all just default state from the tutorial
    Pipeline::Pipeline(Shader& vertexShader, Shader& fragmentShader, Pass& pass)
    {
        // Pipeline info for vertex and fragment shaders
        std::vector<vk::PipelineShaderStageCreateInfo> shaderStages = {
            vk::PipelineShaderStageCreateInfo{
                {},
                vk::ShaderStageFlagBits::eVertex,
                vertexShader.getShaderModule(),
                "main"
            },
            vk::PipelineShaderStageCreateInfo{
                {},
                vk::ShaderStageFlagBits::eFragment,
                fragmentShader.getShaderModule(),
                "main"
            }
        };

        // Vertex input info
        // Right now nothing is here :(
        vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
        vertexInputInfo.vertexBindingDescriptionCount = 0;
        vertexInputInfo.vertexAttributeDescriptionCount = 0;

        // Geometry assembly info
        vk::PipelineInputAssemblyStateCreateInfo assemblyInfo;
        assemblyInfo.topology = vk::PrimitiveTopology::eTriangleList;
        assemblyInfo.primitiveRestartEnable = false;

        // Viewport and scissor settings
        vk::Extent2D swapchainExtents = Context::get().getSwapchain().getSwapchainExtents();

        vk::Viewport viewport;
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(swapchainExtents.width);
        viewport.height = static_cast<float>(swapchainExtents.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        vk::Rect2D scissor;
        scissor.offset.x = 0;
        scissor.offset.y = 0;
        scissor.extent = swapchainExtents;

        vk::PipelineViewportStateCreateInfo viewportState;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        // Rasterizer settings!
        vk::PipelineRasterizationStateCreateInfo rasterState;
        rasterState.depthClampEnable = false;
        rasterState.rasterizerDiscardEnable = false;
        rasterState.polygonMode = vk::PolygonMode::eFill;
        rasterState.cullMode = vk::CullModeFlagBits::eBack;
        rasterState.frontFace = vk::FrontFace::eClockwise;
        rasterState.depthBiasEnable = false;
        rasterState.lineWidth = 1.0f;

        // Multisampling settings :o
        vk::PipelineMultisampleStateCreateInfo multisampleState;
        multisampleState.sampleShadingEnable = false;
        multisampleState.rasterizationSamples = vk::SampleCountFlagBits::e1;

        // Blend settings
        vk::PipelineColorBlendAttachmentState blendAttachment;
        blendAttachment.colorWriteMask =
            vk::ColorComponentFlagBits::eR |
            vk::ColorComponentFlagBits::eG |
            vk::ColorComponentFlagBits::eB;
        blendAttachment.blendEnable = false;

        vk::PipelineColorBlendStateCreateInfo blendState;
        blendState.logicOpEnable = false;
        blendState.attachmentCount = 1;
        blendState.pAttachments = &blendAttachment;

        // Dynamic settings
        std::vector<vk::DynamicState> enabledDynamicStates = {
            // vk::DynamicState::eViewport
        };
        vk::PipelineDynamicStateCreateInfo dynamicState;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(enabledDynamicStates.size());
        dynamicState.pDynamicStates = enabledDynamicStates.data();

        // Create the pipeline layout
        vk::PipelineLayoutCreateInfo layoutInfo;
        spdlog::info("Creating pipeline layout");
        m_pipelineLayout = Context::getVulkanDevice().createPipelineLayoutUnique(layoutInfo);

        // Finally create the pipeline!
        vk::GraphicsPipelineCreateInfo createInfo;
        createInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
        createInfo.pStages = shaderStages.data();
        createInfo.pVertexInputState = &vertexInputInfo;
        createInfo.pInputAssemblyState = &assemblyInfo;
        createInfo.pRasterizationState = &rasterState;
        createInfo.pViewportState = &viewportState;
        createInfo.pMultisampleState = &multisampleState;
        createInfo.pColorBlendState = &blendState;
        createInfo.layout = *m_pipelineLayout;
        createInfo.renderPass = pass.getRenderPass();
        createInfo.subpass = 0;

        spdlog::info("Creating graphics pipeline");
        m_pipeline = Context::getVulkanDevice().createGraphicsPipelineUnique(nullptr, createInfo);
    }

    Pipeline::~Pipeline()
    {
        spdlog::info("Destroying graphics pipeline");
    }
}
