#include "device.hpp"

#include <spdlog/spdlog.h>

#include "instance.hpp"

namespace Rendering
{
    DeviceProperties::DeviceProperties(const vk::PhysicalDevice& physicalDevice) :
        m_physicalDevice(physicalDevice), m_totalHeapSize(0)
    {
        // Query main property structures
        m_deviceProperties = m_physicalDevice.getProperties();
        m_memoryProperties = m_physicalDevice.getMemoryProperties();
        m_queueProperties = m_physicalDevice.getQueueFamilyProperties();

        // Calculate total heap size
        for (uint32_t i = 0; i < m_memoryProperties.memoryHeapCount; i ++)
        {
            m_totalHeapSize += m_memoryProperties.memoryHeaps[i].size;
        }

        // List all of the queues on debug
        spdlog::debug("{} has {} queue families", m_deviceProperties.deviceName, m_queueProperties.size());
        for (auto& i : m_queueProperties)
        {
            spdlog::debug("\t{} queues - {} flags", i.queueCount, static_cast<uint32_t>(i.queueFlags));
        }

        // Find the needed queue families
        // Right now we just need a single graphics queue
        for (size_t i = 0; i < m_queueProperties.size(); i++)
        {
            if (m_queueProperties[i].queueFlags & vk::QueueFlagBits::eGraphics)
            {
                spdlog::debug("\tGraphics queue selected at index {}", i);
                m_graphicsQueue = static_cast<uint32_t>(i);
                break;
            }
        }
    }

    bool DeviceProperties::getSupportsRequiredFeatures() const
    {
        // Check if we have a supported graphics queue
        if (!m_graphicsQueue.has_value())
        {
            return false;
        }

        return true;
    }

    bool operator<(const DeviceProperties& a, const DeviceProperties& b)
    {
        // Prioritize discrete GPUs
        if (a.getDeviceProperties().deviceType != b.getDeviceProperties().deviceType)
        {
            if (a.getDeviceProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
            {
                return true;
            }
            else if (b.getDeviceProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
            {
                return false;
            }
        }

        // Sort by total heap size
        return a.getTotalHeapSize() > b.getTotalHeapSize();
    }

    Device::Device(const DeviceProperties& properties)
    {
        float defaultQueuePriority = 1.0f;
        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfo;

        // Add a request for a graphics queue
        queueCreateInfo.push_back(vk::DeviceQueueCreateInfo{{}, properties.getGraphicsQueue().value(), 1, &defaultQueuePriority});

        // Populate our device info with all requested queues
        vk::DeviceCreateInfo createInfo;
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfo.size());
        createInfo.pQueueCreateInfos = queueCreateInfo.data();

        // Create the device with exception handling
        spdlog::info("Creating Vulkan device for {}", properties.getDeviceProperties().deviceName);
        try
        {
            m_device = properties.getPhysicalDevice().createDeviceUnique(createInfo);
        }
        catch (const std::exception& exception)
        {
            spdlog::error("Failed to create Vulkan device: {}", exception.what());
            throw exception;
        }

        spdlog::info("Aquiring graphics queue");
        m_graphicsQueue = m_device->getQueue(properties.getGraphicsQueue().value(), 0);
    }
}
