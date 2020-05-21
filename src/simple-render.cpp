#include "simple-render.hpp"

#include <iostream>
#include <exception>

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

    m_mainPass.emplace(Rendering::Context::get().getSwapchain());
    m_mainPipeline.emplace(m_mainVertexShader.value(), m_mainFragmentShader.value(), m_mainPass.value());

    m_isRunning = true;
}

SimpleRenderApp::~SimpleRenderApp()
{

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
    }
}

void SimpleRenderApp::render()
{
    Rendering::CommandBuffer primaryBuffer(Rendering::CommandBuffer::Type::Primary);

    vk::CommandBufferBeginInfo primaryBufferInfo;
    primaryBufferInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
    
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
