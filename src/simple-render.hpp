#pragma once

#include <memory>
#include <exception>
#include <optional>

#include <spdlog/spdlog.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.hpp>

#include "rendering/rendering.hpp"

class SimpleRenderApp
{
    public:
        struct FrameData
        {
            FrameData() :
                imageAvailable(nullptr), renderFinished(nullptr),
                fence(nullptr)
            {};

            vk::UniqueSemaphore imageAvailable;
            vk::UniqueSemaphore renderFinished;
            vk::UniqueFence fence;
            vk::UniqueCommandBuffer commandBuffer;
        };

        static const size_t FrameCount = 2;


        SimpleRenderApp();
        ~SimpleRenderApp();

        void loop();
        void render();

    private:
        // Initialization steps
        void initializeLogger();
        void createFrameData();


        bool m_isRunning = false;
        std::shared_ptr<class spdlog::logger> m_mainLogger;

        // Rendering resources
        std::optional<Rendering::Shader> m_mainVertexShader;
        std::optional<Rendering::Shader> m_mainFragmentShader;
        std::optional<Rendering::Pass> m_mainPass;
        std::optional<Rendering::Pipeline> m_mainPipeline;
        std::optional<Rendering::Swapchain> m_swapchain;
        size_t m_currentFrame = 0;
        std::array<FrameData, FrameCount> m_frameData;
};
