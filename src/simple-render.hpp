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

        void initialize();
        void loop();
        void destroy();

    private:
        // Initialization steps
        bool initializeLogger();
        bool initializeSDL();
        bool initializeVulkan();


        bool m_isRunning = false;
        std::shared_ptr<class spdlog::logger> m_mainLogger;
        vk::Instance m_vulkanInstance;
};
