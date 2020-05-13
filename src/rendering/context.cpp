#include "context.hpp"

#include <spdlog/spdlog.h>

namespace Rendering
{
    Context& Context::get()
    {
        static Context context;
        return context;
    }


    Context::Context()
    {
        // Ensure that the instance is always created before the context
        // as the context depends on the instance existing
        Instance::get();
    }

    Context::~Context()
    {
        spdlog::debug("Context destroyed");
    }
}
