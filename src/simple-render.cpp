#include "simple-render.hpp"

#include <iostream>
#include <exception>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "rendering/context.hpp"
#include "rendering/shader.hpp"

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

    Rendering::Shader mainVertexShader("rc/shaders/test_vert.spv");

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



void SimpleRenderApp::initializeLogger()
{
    spdlog::set_level(spdlog::level::trace);

    // Set up a basic logger (don't handle exceptions - main will get those)
    auto defaultLogger = spdlog::basic_logger_mt("default_logger", "log.txt", true);
    spdlog::set_default_logger(defaultLogger);
}
