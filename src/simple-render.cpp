#include "simple-render.hpp"

#include <iostream>
#include <exception>
#include <limits>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

int main()
{
    try
    {
        SimpleRenderApp application;
        application.loop();
    }
    catch (const std::exception& exception)
    {
        spdlog::error("Fatal exception: {}", exception.what());
        std::cout << exception.what() << std::endl;
        return 1;
    }

    return 0;
}



SimpleRenderApp::SimpleRenderApp()
{
    initializeLogger();
    Rendering::Instance::get();
    Rendering::Context::get();

    m_mainVertexShader.emplace("rc/shaders/test_vert.spv");
    m_mainFragmentShader.emplace("rc/shaders/test_frag.spv");

    m_mainPass.emplace();
    m_mainPipeline.emplace(m_mainVertexShader.value(), m_mainFragmentShader.value(), m_mainPass.value());
    m_swapchain.emplace(Rendering::Context::get().getWindow(), m_mainPass.value());
    createFrameData();

    m_isRunning = true;
}

SimpleRenderApp::~SimpleRenderApp()
{
    std::vector<vk::Fence> fences;

    for (auto& i : m_frameData)
    {
        fences.push_back(*i.fence);
    }

    spdlog::info("Waiting for last rendering commands to finish");
    Rendering::Context::getVulkanDevice().waitForFences(fences, true, std::numeric_limits<uint64_t>::max());
}

void SimpleRenderApp::loop()
{
    SDL_Event event;

    while (m_isRunning)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_EventType::SDL_QUIT:
                    m_isRunning = false;
                    break;

                default:
                    break;
            }
        }

        render();
    }
}

void SimpleRenderApp::render()
{
    auto& currentFrameData = m_frameData[m_currentFrame];

    // Wait for the frame command buffer to be free
    Rendering::Context::getVulkanDevice().waitForFences({*currentFrameData.fence}, true,
        std::numeric_limits<uint64_t>::max());

    currentFrameData.commandBuffer->reset({});
    currentFrameData.commandBuffer->begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});

    // Aquire the next swapchain image and signal image available semaphore
    // when it's ready
    uint32_t swapchainImageIndex =
        Rendering::Context::getVulkanDevice().acquireNextImageKHR(
            m_swapchain->getSwapchain(),
            std::numeric_limits<uint64_t>::max(),
            *currentFrameData.imageAvailable,
            nullptr
        ).value;

    auto& swapchainImage =
        m_swapchain.value().getSwapchainImages()[static_cast<size_t>(swapchainImageIndex)];

    // Run our main render pass
    vk::RenderPassBeginInfo renderPassInfo;
    renderPassInfo.renderPass = m_mainPass->getRenderPass();
    renderPassInfo.framebuffer = *swapchainImage.framebuffer;
    renderPassInfo.renderArea.offset = vk::Offset2D({0, 0});
    renderPassInfo.renderArea.extent = m_swapchain->getSwapchainExtents();

    // Clear color to black
    vk::ClearValue clearValue = vk::ClearColorValue(std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f});
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearValue;

    // Record the render pass on the command buffer
    currentFrameData.commandBuffer->beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
    currentFrameData.commandBuffer->setViewport(0, {vk::Viewport{
        0, 0,
        static_cast<float>(m_swapchain->getSwapchainExtents().width), 
        static_cast<float>(m_swapchain->getSwapchainExtents().height),
        0.0f, 1.0f
    }});
    currentFrameData.commandBuffer->setScissor(0, {vk::Rect2D{
        {0, 0},
        m_swapchain->getSwapchainExtents()
    }});
    currentFrameData.commandBuffer->bindPipeline(
        vk::PipelineBindPoint::eGraphics, m_mainPipeline->getPipeline());
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
    presentInfo.pSwapchains = &(m_swapchain->getSwapchain());
    presentInfo.pImageIndices = &swapchainImageIndex;

    Rendering::Context::get().getDevice().getPresentationQueue().presentKHR(
        &presentInfo
    );

    // Increment frame count
    m_currentFrame = (m_currentFrame + 1) % FrameCount;
}



void SimpleRenderApp::initializeLogger()
{
    spdlog::set_level(spdlog::level::trace);

    // Set up a basic logger (don't handle exceptions - main will get those)
    auto defaultLogger = spdlog::basic_logger_mt("default_logger", "log.txt", true);

    // Flush on any error
    defaultLogger->flush_on(spdlog::level::err);
    
    spdlog::set_default_logger(defaultLogger);
}

void SimpleRenderApp::createFrameData()
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
