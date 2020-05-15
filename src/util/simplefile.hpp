#pragma once

#include <string_view>
#include <vector>

namespace Util
{
    class SimpleFile
    {
        public:
            SimpleFile(const std::string_view& filePath);

            const std::vector<char>& getContents() const {
                return m_fileContents;
            }
            
            template <typename T>
            const T* getContentsRaw() const {
                return reinterpret_cast<const T*>(m_fileContents.data());
            }

        private:
            std::vector<char> m_fileContents;
    };
}
