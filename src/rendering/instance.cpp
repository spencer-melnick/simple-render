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


// Allow default Vulkan-Hpp loader
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

// Declaration for debug callback
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData);


namespace Rendering
{
    Instance::Instance()
    {
        initializeSdl();
        initializeVulkan();
        setupVulkanDebug();

        auto extensionProperies = vk::enumerateInstanceExtensionProperties();
        spdlog::info("{} Vulkan extensions supported", extensionProperies.size());
    }

    Instance::~Instance()
    {
        spdlog::info("Quitting SDL");
        SDL_Quit();
    }

    const vk::Instance& Instance::getVulkanInstance() const
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
        // Set up Vulkan dynamic loader to get access to extension functions
        // SDL provides a function to get the vkGetInstanceProcAddress function pointer via its own chosen
        // Vulkan loader
        auto getInstanceProcAddr = static_cast<PFN_vkGetInstanceProcAddr>(SDL_Vulkan_GetVkGetInstanceProcAddr());
        VULKAN_HPP_DEFAULT_DISPATCHER.init(getInstanceProcAddr);

        // Basic application info for driver support
        vk::ApplicationInfo applicationInfo;
        applicationInfo.pApplicationName = "Simple-Render";
        applicationInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
        applicationInfo.pEngineName = "No Engine";
        applicationInfo.engineVersion = VK_MAKE_VERSION(0, 0, 0);
        applicationInfo.apiVersion = VK_API_VERSION_1_2;

        // Get required extensions
        auto extensions = getRequiredExtensions();

        // Query supported validation layers
        auto validationLayers = getSupportedValidationLayers();

        // Instance creation info
        vk::InstanceCreateInfo instanceInfo;
        instanceInfo.pApplicationInfo = &applicationInfo;
        instanceInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        instanceInfo.ppEnabledExtensionNames = extensions.data();
        instanceInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        instanceInfo.ppEnabledLayerNames = validationLayers.data();

        spdlog::info("Creating Vulkan instance");
        try
        {
            m_vulkanInstance = vk::createInstanceUnique(instanceInfo);
        }
        catch (const std::exception& exception)
        {
            // Log exception and throw it higher up
            spdlog::error("Failed to create Vulkan instance: {}", exception.what());
            throw exception;
        }

        // Initialize default dispatcher with our Vulkan instance to get access to
        // instance specific functions
        VULKAN_HPP_DEFAULT_DISPATCHER.init(*m_vulkanInstance);
    }

    std::vector<const char*> Instance::getRequiredExtensions()
    {
        // Query extensions required for rendering to SDL window
        unsigned int sdlRequiredExtensionCount;
        SDL_Vulkan_GetInstanceExtensions(nullptr, &sdlRequiredExtensionCount, nullptr);
        const char** sdlRequiredExtensionNames = new const char*[sdlRequiredExtensionCount];
        SDL_Vulkan_GetInstanceExtensions(nullptr, &sdlRequiredExtensionCount, sdlRequiredExtensionNames);

        std::vector<const char*> requiredExtensions;

        // Request the debug trace extension in debug builds
        #ifndef NDEBUG
        requiredExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        #endif

        // Add the SDL extensions to the full list
        for (unsigned int i = 0; i < sdlRequiredExtensionCount; i++)
        {
            requiredExtensions.push_back(sdlRequiredExtensionNames[i]);
        }

        return requiredExtensions;
    }

    std::vector<const char*> Instance::getSupportedValidationLayers()
    {
        #ifdef NDEBUG
        return std::vector<const char*>();
        #else
        // Request the Khronos validation in debug builds
        static const std::vector<std::string> requestedLayers = {
            "VK_LAYER_KHRONOS_validation"
        };

        std::vector<const char*> resultLayers;

        // Check to see if the validation layer is actually supported before requesting it
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

    void Instance::setupVulkanDebug()
    {
        spdlog::info("Creating Vulkan debug messenger");

        m_vulkanDebugMessenger = m_vulkanInstance->createDebugUtilsMessengerEXTUnique(
            vk::DebugUtilsMessengerCreateInfoEXT{
                {},
                vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
                    vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                    vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                    vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo,
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
            debugCallback}
        );
    }
}



// Disable warning about unreference formal parameters
// 
#pragma warning(push)
#pragma warning(disable : 4100)

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    switch (static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(messageSeverity))
    {
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose:
            spdlog::debug("Vulkan: {}", pCallbackData->pMessage);
            break;
        
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo:
            spdlog::info("Vulkan: {}", pCallbackData->pMessage);
            break;

        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
            spdlog::warn("Vulkan: {}", pCallbackData->pMessage);
            break;

        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
            spdlog::error("Vulkan: {}", pCallbackData->pMessage);
            break;
    }

    return VK_FALSE;
}

#pragma warning(pop)

