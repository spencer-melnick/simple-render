#include "pass.hpp"

#include <spdlog/spdlog.h>

#include "context.hpp"

namespace Rendering
{
    Pass::Pass()
    {
        // Right now this is a very simple render pass, just base color
        vk::AttachmentDescription colorAttachment;
        colorAttachment.format = Context::get().getSwapchain().getSurfaceFormat().format;
        colorAttachment.samples = vk::SampleCountFlagBits::e1;
        colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
        colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
        colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
        colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

        // Fragment color is layout = 0!
        vk::AttachmentReference colorAttachmentReference;
        colorAttachmentReference.layout = vk::ImageLayout::eColorAttachmentOptimal;
        colorAttachmentReference.attachment = 0;

        // Single subpass
        vk::SubpassDescription colorPass;
        colorPass.colorAttachmentCount = 1;
        colorPass.pColorAttachments = &colorAttachmentReference;

        // Creation info
        vk::RenderPassCreateInfo createInfo;
        createInfo.attachmentCount = 1;
        createInfo.pAttachments = &colorAttachment;
        createInfo.subpassCount = 1;
        createInfo.pSubpasses = &colorPass;

        spdlog::info("Creating rendering pass");
        m_renderPass = Context::getVulkanDevice().createRenderPassUnique(createInfo);
    }

    Pass::~Pass()
    {
        spdlog::info("Destroying render pass");
    }
}
