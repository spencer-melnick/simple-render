#include "simple-render.hpp"

#include <iostream>
#include <exception>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "rendering/context.hpp"

int main()
{
    try
    {
        SimpleRenderApp application;
    }
    catch (const std::exception& exception)
    {
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
}

SimpleRenderApp::~SimpleRenderApp()
{

}

void SimpleRenderApp::loop()
{
    // while (m_isRunning)
    // {

    // }
}



void SimpleRenderApp::initializeLogger()
{
    spdlog::set_level(spdlog::level::trace);

    // Set up a basic logger (don't handle exceptions - main will get those)
    auto defaultLogger = spdlog::basic_logger_mt("default_logger", "log.txt", true);
    spdlog::set_default_logger(defaultLogger);
}
