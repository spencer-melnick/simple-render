#pragma once

#include <optional>

#include <vulkan/vulkan.hpp>

#include "instance.hpp"
#include "window.hpp"
#include "device.hpp"
#include "swapchain.hpp"

namespace Rendering
{
    // Context is a singleton that holds all of the global information needed for rendering that would
    // be otherwise difficult to pass through the entire program
    class Context
    {
        public:
            static Context& get();

            Device& getDevice() {
                return m_device.value();
            }
            Window& getWindow() {
                return m_window.value();
            }
            Swapchain& getSwapchain() {
                return m_swapchain.value();
            }
            static const vk::CommandPool& getCommandPool() {
                return *get().m_commandPool;
            }
            static const vk::Device& getVulkanDevice() {
                return get().getDevice().getVulkanDevice();
            }
        
        private:
            Context();
            ~Context();

            // Initialization steps
            void chooseDevice();
            void createCommandPool();

            std::optional<Window> m_window;
            std::optional<Device> m_device;
            std::optional<Swapchain> m_swapchain;
            vk::UniqueCommandPool m_commandPool;
    };
}
