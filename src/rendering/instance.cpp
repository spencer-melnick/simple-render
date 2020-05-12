#include "instance.hpp"

// Standard libraries
#include <exception>
#include <vector>
#include <string>

// 3rd party libraries
#include <spdlog/spdlog.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.hpp>

namespace Rendering
{
    Instance::Instance()
    {
        m_vulkanInstance = new vk::Instance(nullptr);

        initializeSdl();
        initializeVulkan();

        auto extensionProperies = vk::enumerateInstanceExtensionProperties();
        spdlog::info("{} Vulkan extensions supported", extensionProperies.size());
    }

    Instance::~Instance()
    {
        spdlog::info("Destroying Vulkan instance");
        m_vulkanInstance->destroy();
        delete m_vulkanInstance;

        spdlog::info("Quitting SDL");
        SDL_Quit();
    }

    vk::Instance& Instance::getVulkanInstance() const
    {
        return *m_vulkanInstance;
    }

    void Instance::initializeSdl()
    {
        spdlog::info("Intializing SDL");
        if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        {
            spdlog::error("SDL failed to initialize: {}", SDL_GetError());
            throw std::exception("SDL initialization failure");
        }

        spdlog::info("Loading Vulkan library");
        if (SDL_Vulkan_LoadLibrary(nullptr) != 0)
        {
            spdlog::error("Failed to load Vulkan library: {}", SDL_GetError());
            throw std::exception("SDL Vulkan library load failure");
        }
    }

    void Instance::initializeVulkan()
    {
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

        // Query supported validation layers
        auto validationLayers = getSupportedValidationLayers();

        // Instance creation info
        vk::InstanceCreateInfo instanceInfo;
        instanceInfo.pApplicationInfo = &applicationInfo;
        instanceInfo.enabledExtensionCount = requiredExtensionCount;
        instanceInfo.ppEnabledExtensionNames = requiredExtensionNames;
        instanceInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        instanceInfo.ppEnabledLayerNames = validationLayers.data();

        spdlog::info("Creating Vulkan instance");
        try
        {
            *m_vulkanInstance = vk::createInstance(instanceInfo);
        }
        catch (const std::exception& exception)
        {
            // Log exception and throw it higher up
            spdlog::error("Failed to create Vulkan instance: {}", exception.what());
            throw exception;
        }
    }

    std::vector<const char*> Instance::getSupportedValidationLayers()
    {
        #ifdef NDEBUG
            return std::vector<const char*>();
        #else
            static const std::vector<std::string> requestedLayers = {
                "VK_LAYER_KHRONOS_validation"
            };

            std::vector<const char*> resultLayers;

            auto supportedLayers = vk::enumerateInstanceLayerProperties();
            for (auto& i : requestedLayers)
            {
                for (auto& j : supportedLayers)
                {
                    if (i == j.layerName)
                    {
                        // Vulkan creation info requires a C string, so we'll convert here
                        resultLayers.push_back(i.c_str());
                        break;
                    }
                }
            }

            spdlog::info("{} of {} requested Vulkan validation layers supported", requestedLayers.size(), resultLayers.size());
            return resultLayers;
        #endif
    }
}
