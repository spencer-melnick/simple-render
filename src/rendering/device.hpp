#pragma once

#include <optional>

#include <vulkan/vulkan.hpp>

namespace Rendering
{
    // Queries and stores Vulkan physical device properties in addition
    // to computing some metrics for determining optimal device
    // and feature support
    class DeviceProperties
    {
        public:
            DeviceProperties(const vk::PhysicalDevice& physicalDevice);


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
            auto getGraphicsQueue() const {
                return m_graphicsQueue;
            }

            bool getSupportsRequiredFeatures() const;


        private:
            vk::PhysicalDevice m_physicalDevice;
            vk::PhysicalDeviceProperties m_deviceProperties;
            vk::PhysicalDeviceMemoryProperties m_memoryProperties;
            vk::DeviceSize m_totalHeapSize;
            std::vector<vk::QueueFamilyProperties> m_queueProperties;
            std::optional<uint32_t> m_graphicsQueue;
    };

    // Sort operator
    bool operator<(const DeviceProperties& a, const DeviceProperties& b);


    // Simple wrapper for a logical Vulkan device
    class Device
    {
        public:
            Device(const DeviceProperties& properties);

            const vk::Device& getVulkanDevice() const {
                return *m_device;
            }

            vk::Queue& getGraphicsQueue() {
                return m_graphicsQueue;
            }

        private:
            vk::UniqueDevice m_device;
            vk::Queue m_graphicsQueue;
    };
}
