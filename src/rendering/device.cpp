#include "device.hpp"

#include <set>
#include <string_view>

#include <spdlog/spdlog.h>

#include "instance.hpp"

namespace Rendering
{
    std::vector<const char*> requiredDeviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    vk::Format defaultSurfaceColorFormat = vk::Format::eB8G8R8A8Unorm;
    


    DeviceProperties::DeviceProperties(const vk::PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface) :
        m_physicalDevice(physicalDevice), m_totalHeapSize(0)
    {
        // Query main property structures
        m_deviceProperties = m_physicalDevice.getProperties();
        m_memoryProperties = m_physicalDevice.getMemoryProperties();
        m_queueProperties = m_physicalDevice.getQueueFamilyProperties();
        m_extensionProperties = m_physicalDevice.enumerateDeviceExtensionProperties();
        m_surfaceFormats = m_physicalDevice.getSurfaceFormatsKHR(surface);
        m_presentModes = m_physicalDevice.getSurfacePresentModesKHR(surface);

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

        // Find the first available graphics queue family
        for (size_t i = 0; i < m_queueProperties.size(); i++)
        {
            if (m_queueProperties[i].queueFlags & vk::QueueFlagBits::eGraphics)
            {
                spdlog::debug("\tGraphics queue selected at index {}", i);
                m_graphicsQueue = static_cast<uint32_t>(i);
                break;
            }
        }

        // Find the first available presentation queue family
        for (size_t i = 0; i < m_queueProperties.size(); i++)
        {
            if (m_physicalDevice.getSurfaceSupportKHR(static_cast<uint32_t>(i), surface))
            {
                spdlog::debug("\tPresentation queue selected at index {}", i);
                m_presentationQueue = static_cast<uint32_t>(i);
                break;
            }
        }
    }

    bool DeviceProperties::getSupportsRequiredFeatures() const
    {
        // Check if we have a supported graphics queue
        if (!m_graphicsQueue.has_value() || !m_presentationQueue.has_value())
        {
            return false;
        }

        // Check if our device has all of the required extensions
        for (auto& i : requiredDeviceExtensions)
        {
            bool wasExtensionFound = false;
            std::string_view extensionName(i);

            for (auto& j : m_extensionProperties)
            {
                if (j.extensionName == extensionName)
                {
                    wasExtensionFound = true;
                    break;
                }
            }

            if (!wasExtensionFound)
            {
                return false;
            }
        }

        // Check for supported formats
        if (m_surfaceFormats.empty() || m_presentModes.empty())
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

    Device::Device(DeviceProperties&& properties) :
        m_properties(std::move(properties))
    {
        if (!m_properties.getSupportsRequiredFeatures())
        {
            spdlog::error("Cannot create logical device for {} - does not support required features",
                m_properties.getDeviceProperties().deviceName);
            throw std::exception("Physical device does not support required Vulkan features");
        }

        float defaultQueuePriority = 1.0f;
        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfo;

        // Add a request for graphics and presentation queues
        std::set<uint32_t> requestedQueueFamilies = {m_properties.getGraphicsQueue(), m_properties.getPresentationQueue()};

        for (auto& i : requestedQueueFamilies)
        {
            queueCreateInfo.push_back(vk::DeviceQueueCreateInfo{{}, m_properties.getGraphicsQueue(), 1, &defaultQueuePriority});
        }

        // Populate our device info with all requested queues
        vk::DeviceCreateInfo createInfo;
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfo.size());
        createInfo.pQueueCreateInfos = queueCreateInfo.data();

        // Add all required device extensions
        createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredDeviceExtensions.size());
        createInfo.ppEnabledExtensionNames = requiredDeviceExtensions.data();

        // Create the device with exception handling
        spdlog::info("Creating Vulkan device for {}", m_properties.getDeviceProperties().deviceName);
        try
        {
            m_device = m_properties.getPhysicalDevice().createDeviceUnique(createInfo);
        }
        catch (const std::exception& exception)
        {
            spdlog::error("Failed to create Vulkan device: {}", exception.what());
            throw exception;
        }

        spdlog::info("Aquiring queues");
        m_graphicsQueue = m_device->getQueue(m_properties.getGraphicsQueue(), 0);
        m_presentationQueue = m_device->getQueue(m_properties.getPresentationQueue(), 0);

        chooseSurfaceFormat();
    }

    Device::~Device()
    {
        spdlog::info("Destroying Vulkan logical device");
    }

    void Device::chooseSurfaceFormat()
    {
        auto& surfaceFormats = m_properties.getSurfaceFormats();

        // If there is a single supported format with the type undefined
        // then any surface format is supported
        if (surfaceFormats.size() == 1 &&
            surfaceFormats.front().format == vk::Format::eUndefined)
        {
            m_surfaceFormat.format = vk::Format::eR8G8B8Unorm;
            m_surfaceFormat.colorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;
        }
        // Otherwise just use the suggested format
        else
        {
            m_surfaceFormat = surfaceFormats.front();
        }
    }
}
