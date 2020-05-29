#include "renderer.hpp"

#include <limits>

#include <spdlog/spdlog.h>

#include "context.hpp"
#include "swapchain.hpp"
#include "pass.hpp"
#include "pipeline.hpp"
#include "commands.hpp"

namespace Rendering
{
    Renderer::Renderer()
    {
        spdlog::info("Creating renderer");

        createCommandPools();
        createFrameData();
    }

    Renderer::~Renderer()
    {
        spdlog::info("Destroying renderer");

        waitForCommandsFree();
    }


    // Main functions

    void Renderer::render(Swapchain& swapchain, const Pass& pass,
        const Pipeline& pipeline)
    {
        auto& currentFrameData = m_frameData[m_currentFrame];

        // Wait for the frame command buffer to be free
        Rendering::Context::getVulkanDevice().waitForFences({*currentFrameData.fence}, true,
            (std::numeric_limits<uint64_t>::max)());

        // Clear the command buffer and set it up for fast re-recording
        currentFrameData.commandBuffer->reset({});
        currentFrameData.commandBuffer->begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});

        // Aquire the next swapchain image
        auto swapchainResult = swapchain.getNextImage(*currentFrameData.imageAvailable);
        if (std::get<0>(swapchainResult) == Swapchain::ImageAquisitionError::ShouldRecreateSwapchain)
        {
            spdlog::info("Recreating swapchain");
            // Do something here
            throw std::exception("Failed to recreate swapchain");
        }

        const Swapchain::Image& swapchainImage = std::get<1>(swapchainResult)->get();

        // Run our main render pass
        vk::RenderPassBeginInfo renderPassInfo;
        renderPassInfo.renderPass = pass.getRenderPass();
        renderPassInfo.framebuffer = *swapchainImage.framebuffer;
        renderPassInfo.renderArea.offset = vk::Offset2D({0, 0});
        renderPassInfo.renderArea.extent = swapchain.getSwapchainExtents();

        // Clear color to black
        vk::ClearValue clearValue = vk::ClearColorValue(std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f});
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearValue;

        // Record the render pass on the command buffer
        currentFrameData.commandBuffer->beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
        Commands::setScissorAndViewport(*currentFrameData.commandBuffer,
            swapchain.getSwapchainExtents().width, swapchain.getSwapchainExtents().height);
        currentFrameData.commandBuffer->bindPipeline(
            vk::PipelineBindPoint::eGraphics, pipeline.getPipeline());
        currentFrameData.commandBuffer->draw(3, 1, 0, 0);
        currentFrameData.commandBuffer->endRenderPass();
        currentFrameData.commandBuffer->end();

        // Submit the command buffer
        vk::SubmitInfo submitInfo;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &(*currentFrameData.commandBuffer);

        // Wait until the current frame image is ready before drawing
        vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        submitInfo.pWaitDstStageMask = &waitStage;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &(*currentFrameData.imageAvailable);

        // Notify the render finished semaphore on completion
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &(*currentFrameData.renderFinished);

        // Fence will be signalled on buffer completion
        Rendering::Context::getVulkanDevice().resetFences({*currentFrameData.fence});
        Rendering::Context::get().getDevice().getGraphicsQueue().submit(
            {submitInfo},
            *currentFrameData.fence
        );

        // Present the image
        vk::PresentInfoKHR presentInfo;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &(*currentFrameData.renderFinished);
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &(swapchain.getSwapchain());
        presentInfo.pImageIndices = &swapchainImage.index;

        Rendering::Context::get().getDevice().getPresentationQueue().presentKHR(
            &presentInfo
        );

        // Increment frame count
        m_currentFrame = (m_currentFrame + 1) % FrameCount;
    }

    void Renderer::waitForCommandsFree()
    {
        std::vector<vk::Fence> fences;

        for (auto& i : m_frameData)
        {
            fences.push_back(*i.fence);
        }

        spdlog::info("Waiting for rendering commands to finish");
        Context::getVulkanDevice().waitForFences(fences, true, (std::numeric_limits<uint64_t>::max)());
    }



    // Initialization steps

    void Renderer::createCommandPools()
    {
        vk::CommandPoolCreateInfo createInfo;
        createInfo.flags = vk::CommandPoolCreateFlagBits::eTransient | vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
        createInfo.queueFamilyIndex = Context::get().getDevice().getProperties().getGraphicsQueue();

        spdlog::info("Creating primary command pool");
        m_primaryCommandPool = Context::getVulkanDevice().createCommandPoolUnique(createInfo);
    }

    void Renderer::createFrameData()
    {
        spdlog::info("Creating frame data for {} frames", m_frameData.size());
        for (auto& i : m_frameData)
        {
            i.imageAvailable = Rendering::Context::getVulkanDevice().createSemaphoreUnique({});
            i.renderFinished = Rendering::Context::getVulkanDevice().createSemaphoreUnique({});
            i.fence = Rendering::Context::getVulkanDevice().createFenceUnique({vk::FenceCreateFlagBits::eSignaled});
            
            i.commandBuffer = std::move(Rendering::Context::getVulkanDevice().allocateCommandBuffersUnique({
                Rendering::Context::get().getCommandPool(),
                vk::CommandBufferLevel::ePrimary,
                1
            }).front());
        }
    }
}
