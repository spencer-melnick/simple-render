#include "context.hpp"

namespace Rendering
{
    Context& Context::get()
    {
        static Context context;
        return context;
    }

    Instance& Context::getInstance()
    {
        return m_instance;
    }


    Context::Context()
    {

    }

    Context::~Context()
    {

    }
}
