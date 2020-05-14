#pragma once

#include <optional>
#include <vector>

#include <vulkan/vulkan.hpp>

namespace Rendering
{
    extern std::vector<const char*> requiredDeviceExtensions;
    extern vk::Format defaultSurfaceColorFormat;

    // Queries and stores Vulkan physical device properties in addition
    // to computing some metrics for determining optimal device
    // and feature support
    class DeviceProperties
    {
        public:
            DeviceProperties(const vk::PhysicalDevice& physicalDevice,
                const vk::SurfaceKHR& surface);


            // Getters
            auto getPhysicalDevice() const {
                return m_physicalDevice;
            }
            auto getDeviceProperties() const {
                return m_deviceProperties;
            }
            auto getMemoryProperties() const {
                return m_memoryProperties;
            }
            auto getTotalHeapSize() const {
                return m_totalHeapSize;
            }
            auto getQueueProperties() const {
                return m_queueProperties;
            }
            auto getExtensionProperties() const {
                return m_extensionProperties;
            }
            auto getGraphicsQueue() const {
                return m_graphicsQueue.value();
            }
            auto getPresentationQueue() const {
                return m_presentationQueue.value();
            }

            bool getSupportsRequiredFeatures() const;


        private:
            vk::PhysicalDevice m_physicalDevice;
            vk::PhysicalDeviceProperties m_deviceProperties;
            vk::PhysicalDeviceMemoryProperties m_memoryProperties;
            vk::DeviceSize m_totalHeapSize;
            std::vector<vk::QueueFamilyProperties> m_queueProperties;
            std::vector<vk::ExtensionProperties> m_extensionProperties;
            std::vector<vk::SurfaceFormatKHR> m_surfaceFormats;
            std::vector<vk::PresentModeKHR> m_presentModes;
            std::optional<uint32_t> m_graphicsQueue;
            std::optional<uint32_t> m_presentationQueue;
    };

    // Sort operator
    bool operator<(const DeviceProperties& a, const DeviceProperties& b);


    // Simple wrapper for a logical Vulkan device
    class Device
    {
        public:
            Device(DeviceProperties&& properties);
            ~Device();

            const DeviceProperties getProperties() const {
                return m_properties;
            }
            const vk::Device& getVulkanDevice() const {
                return *m_device;
            }
            vk::Queue& getGraphicsQueue() {
                return m_graphicsQueue;
            }
            vk::Queue& getPresentationQueue() {
                return m_presentationQueue;
            }

        private:
            DeviceProperties m_properties;
            vk::UniqueDevice m_device;
            vk::Queue m_graphicsQueue;
            vk::Queue m_presentationQueue;
    };
}
