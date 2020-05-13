#include "context.hpp"

#include <algorithm>

#include <spdlog/spdlog.h>

namespace Rendering
{
    Context& Context::get()
    {
        static Context context;
        return context;
    }


    Context::Context()
    {
        // Ensure that the instance is always created before the context
        // as the context depends on the instance existing
        Instance::get();

        chooseDevice();
    }

    Context::~Context()
    {
        spdlog::debug("Context destroyed");
    }

    void Context::chooseDevice()
    {
        auto physicalDevices = Instance::get().getVulkanInstance().enumeratePhysicalDevices();

        if (physicalDevices.empty())
        {
            spdlog::error("No graphics devices with Vulkan support");
            throw std::exception("No graphics devices with Vulkan support");
        }

        spdlog::info("{} graphics devices with Vulkan support found", physicalDevices.size());

        // Remove devices that don't support required features
        physicalDevices.erase(std::remove_if(physicalDevices.begin(), physicalDevices.end(), [](const vk::PhysicalDevice& device)
        {
            auto queueFamilies = device.getQueueFamilyProperties();

            for (auto& i : queueFamilies)
            {
                // Must support graphics queue
                if (i.queueFlags & vk::QueueFlagBits::eGraphics)
                {
                    return false;
                }
            }

            return true;
        }), physicalDevices.end());

        // Error if no devices support features
        if (physicalDevices.empty())
        {
            spdlog::error("No devices with required Vulkan features found");
            throw std::exception("No devices with required Vulkan features found");
        }

        spdlog::info("{} devices support required Vulkan features", physicalDevices.size());

        // Order devices by features
        std::sort(physicalDevices.begin(), physicalDevices.end(), [](const vk::PhysicalDevice& a, const vk::PhysicalDevice& b)
        {
            auto aProperties = a.getProperties();
            auto bProperties = b.getProperties();

            // Priortize discrete GPUs
            if (aProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu &&
                bProperties.deviceType != vk::PhysicalDeviceType::eDiscreteGpu)
            {
                return true;
            }
            else if (aProperties.deviceType != vk::PhysicalDeviceType::eDiscreteGpu &&
                bProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
            {
                return false;
            }

            // Next sort by total heap size
            auto aMemoryProperties = a.getMemoryProperties();
            auto bMemoryProperties = b.getMemoryProperties();

            vk::DeviceSize aMemorySize = 0;
            vk::DeviceSize bMemorySize = 0;

            // Sum all heap sizes
            for (uint32_t i = 0; i < aMemoryProperties.memoryHeapCount; i++)
            {
                aMemorySize += aMemoryProperties.memoryHeaps[i].size;
            }
            for (uint32_t i = 0; i < bMemoryProperties.memoryHeapCount; i++)
            {
                bMemorySize += bMemoryProperties.memoryHeaps[i].size;
            }

            return aMemorySize > bMemorySize;
        });

        spdlog::debug("Vulkan devices in order of estimated performance:");

        // List devices in order with name and driver version
        for (auto& i : physicalDevices)
        {
            auto deviceProperties = i.getProperties();
            spdlog::debug("\t{} - Driver Version {}", deviceProperties.deviceName, deviceProperties.driverVersion);
        }
    }
}
