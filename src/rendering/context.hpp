#pragma once

#include <vulkan/vulkan.hpp>

#include "instance.hpp"
#include "device.hpp"

namespace Rendering
{
    // Context is a singleton that holds all of the global information needed for rendering that would
    // be otherwise difficult to pass through the entire program
    class Context
    {
        public:
            static Context& get();

            Instance& getInstance();
        
        private:
            Context();
            ~Context();

            // Initialization steps
            void chooseDevice();
    };
}
