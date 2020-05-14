#pragma once

#include <memory>
#include <exception>
#include <optional>

#include <spdlog/spdlog.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.hpp>

#include "rendering/window.hpp"

class SimpleRenderApp
{
    public:
        SimpleRenderApp();
        ~SimpleRenderApp();

        void loop();

    private:
        // Initialization steps
        void initializeLogger();


        bool m_isRunning = false;
        std::shared_ptr<class spdlog::logger> m_mainLogger;
        std::optional<Rendering::Window> m_window;
};
