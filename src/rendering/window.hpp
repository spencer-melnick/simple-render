#pragma once

#include <SDL2/SDL.h>
#include <vulkan/vulkan.hpp>

namespace Rendering
{
    class Window
    {
        public:
            Window(int width, int height);
            ~Window();

        private:
            SDL_Window* m_window;
            vk::SurfaceKHR m_surface;
    };
}
