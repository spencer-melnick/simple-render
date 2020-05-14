#pragma once

#include <SDL2/SDL.h>
#include <vulkan/vulkan.hpp>

namespace Rendering
{
    class Context;

    class Window
    {
        public:
            Window(int width, int height);
            ~Window() noexcept;

            SDL_Window* getSdlWindow() {
                return m_window;
            }
            vk::SurfaceKHR& getSurface() {
                return m_surface;
            }

        private:
            SDL_Window* m_window;
            vk::SurfaceKHR m_surface;
    };
}
