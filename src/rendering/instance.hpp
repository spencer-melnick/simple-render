#pragma once

// Defines a singleton to access a Vulkan instance
// Also hooks in SDL initialization as it is required for Vulkan use

#include <vector>

// Forward declare vk::Instance
namespace vk
{
    class Instance;
}

namespace Rendering
{
    class Instance
    {
        public:
            static Instance& get();

        protected:
            vk::Instance& getVulkanInstance() const;

        private:
            Instance();
            ~Instance();

            // Initialization steps
            void initializeSdl();
            void initializeVulkan();
            std::vector<const char*> getSupportedValidationLayers();

            vk::Instance* m_vulkanInstance;
    };
}