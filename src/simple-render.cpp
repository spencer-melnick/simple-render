#include "simple-render.hpp"

#include <iostream>
#include <exception>

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <vulkan/vulkan.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

int main()
{
    try
    {
        SimpleRenderApp application;
        application.initialize();
        application.destroy();
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
}

SimpleRenderApp::~SimpleRenderApp()
{

}

void SimpleRenderApp::initialize()
{
    if (initializeLogger() && initializeSDL() && initializeVulkan())
    {
        m_isRunning = true;
    }
}

void SimpleRenderApp::loop()
{
    // while (m_isRunning)
    // {

    // }
}



bool SimpleRenderApp::initializeLogger()
{
    spdlog::set_level(spdlog::level::trace);

    // Set up a basic logger (don't handle exceptions - main will get those)
    auto defaultLogger = spdlog::basic_logger_mt("default_logger", "log.txt");
    spdlog::set_default_logger(defaultLogger);

    return true;
}

bool SimpleRenderApp::initializeSDL()
{
    // Initialize window in steps
    spdlog::info("Intializing SDL");
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        spdlog::error("SDL failed to initialize: {}", SDL_GetError());
        return false;
    }

    return true;
}

bool SimpleRenderApp::initializeVulkan()
{
    spdlog::info("Loading Vulkan library");
    if (SDL_Vulkan_LoadLibrary(nullptr) != 0)
    {
        spdlog::error("Failed to load Vulkan library: {}", SDL_GetError());
        return false;
    }

    auto extensionProperies = vk::enumerateInstanceExtensionProperties();
    spdlog::info("{} Vulkan extensions supported", extensionProperies.size());

    // Basic application info for driver support
    vk::ApplicationInfo applicationInfo;
    applicationInfo.pApplicationName = "Simple-Render";
    applicationInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
    applicationInfo.pEngineName = "No Engine";
    applicationInfo.engineVersion = VK_MAKE_VERSION(0, 0, 0);
    applicationInfo.apiVersion = VK_API_VERSION_1_2;

    // Get required extensions
    unsigned int requiredExtensionCount;
    SDL_Vulkan_GetInstanceExtensions(nullptr, &requiredExtensionCount, nullptr);
    const char** requiredExtensionNames = new const char*[requiredExtensionCount];
    SDL_Vulkan_GetInstanceExtensions(nullptr, &requiredExtensionCount, requiredExtensionNames);

    // Instance creation info
    vk::InstanceCreateInfo instanceInfo;
    instanceInfo.pApplicationInfo = &applicationInfo;
    instanceInfo.enabledExtensionCount = requiredExtensionCount;
    instanceInfo.ppEnabledExtensionNames = requiredExtensionNames;
    instanceInfo.enabledLayerCount = 0;

    spdlog::info("Creating Vulkan instance");
    try
    {
        m_vulkanInstance = vk::createInstance(instanceInfo);
    }
    catch (const std::exception& exception)
    {
        spdlog::error("Failed to create Vulkan instance: {}", exception.what());
        return false;
    }

    return true;
}

void SimpleRenderApp::destroy()
{
    m_vulkanInstance.destroy();
    SDL_Quit();
}
