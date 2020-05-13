#pragma once

// Defines a singleton to access a Vulkan instance
// Also hooks in SDL initialization as it is required for Vulkan use

#include <vulkan/vulkan.hpp>

namespace Rendering
{
    class Context;

    class Instance
    {
        friend class Context;

        protected:
            Instance();
            ~Instance();

            const vk::Instance& getVulkanInstance() const;

        private:
            // Initialization steps
            void initializeSdl();
            void initializeVulkan();
            void setupVulkanDebug();

            // Initialization helpers
            std::vector<const char*> getRequiredExtensions();
            std::vector<const char*> getSupportedValidationLayers();

            vk::UniqueInstance m_vulkanInstance;
            vk::UniqueDebugUtilsMessengerEXT m_vulkanDebugMessenger;
    };
}