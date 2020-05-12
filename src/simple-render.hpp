#pragma once

#include <memory>
#include <exception>

#include <spdlog/spdlog.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.hpp>

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
};
