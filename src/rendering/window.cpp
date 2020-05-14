#include "window.hpp"

#include <exception>

#include <spdlog/spdlog.h>
#include <SDL2/SDL_vulkan.h>

#include "instance.hpp"

namespace Rendering
{
    Window::Window(int width, int height)
    {
        // Create SDL window
        spdlog::info("Creating SDL window of size {}x{}", width, height);
        m_window = SDL_CreateWindow("Simple-Render", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            width, height, SDL_WINDOW_VULKAN);

        // Use C style error checking
        if (m_window == nullptr)
        {
            spdlog::error("Failed to create SDL window: {}", SDL_GetError());
            throw std::exception("SDL window creation failed");
        }

        // Create Vulkan surface directly from window using SDL
        spdlog::info("Creating Vulkan surface for window");
        VkSurfaceKHR rawSurface;
         if (SDL_Vulkan_CreateSurface(m_window, Instance::get().getVulkanInstance(),
            &rawSurface) == SDL_FALSE)
        {
            spdlog::error("Failed to create Vulkan surface: {}", SDL_GetError());
            throw std::exception("Vulkan surface creation failed");
        }

        // Assign C style surface to Vulkan-Hpp handle
        m_surface = vk::SurfaceKHR(rawSurface);
    }

    Window::~Window()
    {
        // We need to destroy the surface before our window
        Instance::get().getVulkanInstance().destroySurfaceKHR(m_surface);

        spdlog::info("Destroying window");
        SDL_DestroyWindow(m_window);
    }
}
