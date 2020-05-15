#include "simplefile.hpp"

#include <fstream>
#include <exception>

#include <spdlog/spdlog.h>

namespace Util
{
    SimpleFile::SimpleFile(const std::string_view& filePath)
    {
        std::ifstream file(filePath.data(), std::ios::in | std::ios::binary);

        if (!file.is_open())
        {
            spdlog::error("Cannot open file \"{}\"", filePath);
            throw std::runtime_error("Cannot open file");
        }

        m_fileContents.insert(m_fileContents.end(), std::istreambuf_iterator<char>(file),
            std::istreambuf_iterator<char>());
    }
}
