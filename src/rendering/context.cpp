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

        // We also need to create a window to get a Vulkan surface
        // and determine which devices support that surface
        m_window.emplace(800, 600);
        chooseDevice();

        // Right now there is a single graphics command pool
        createCommandPool();

        spdlog::info("Rendering context created");
    }

    Context::~Context()
    {
        spdlog::info("Destroying rendering context");
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

        // Get device properties using property wrapper
        std::vector<DeviceProperties> deviceProperties;
        deviceProperties.reserve(physicalDevices.size());
        for (auto& i : physicalDevices)
        {
            deviceProperties.emplace_back(i, m_window->getSurface());
        }

        // Remove devices that don't support required features
        deviceProperties.erase(std::remove_if(deviceProperties.begin(), deviceProperties.end(), [](const DeviceProperties& properties)
        {
            return !properties.getSupportsRequiredFeatures();
        }), deviceProperties.end());

        // Error if no devices support required features
        if (deviceProperties.empty())
        {
            spdlog::error("No graphics devices support required Vulkan features");
            throw std::exception("No graphics devices support required Vulkan features");
        }
        spdlog::info("{} graphics devices with required Vulkan features found", deviceProperties.size());

        spdlog::debug("Vulkan devices in order of estimated performance:");

        // List devices in order with name and driver version
        for (auto& i : deviceProperties)
        {
            spdlog::debug("\t{}", i.getDeviceProperties().deviceName);
        }

        m_device.emplace(std::move(deviceProperties.front()));
        VULKAN_HPP_DEFAULT_DISPATCHER.init(m_device->getVulkanDevice());
    }

    void Context::createCommandPool()
    {
        vk::CommandPoolCreateInfo createInfo;
        createInfo.flags = vk::CommandPoolCreateFlagBits::eTransient | vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
        createInfo.queueFamilyIndex = m_device.value().getProperties().getGraphicsQueue();

        spdlog::info("Creating primary command pool");
        m_commandPool = m_device.value().getVulkanDevice().createCommandPoolUnique(createInfo);
    }
}
