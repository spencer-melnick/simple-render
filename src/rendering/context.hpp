#pragma once

#include "instance.hpp"

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

            // Order matters - these members will be constructed in this order,
            // and destroyed in the inverse order
            Instance m_instance;
    };
}
