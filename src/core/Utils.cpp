#include "core/Utils.hpp"
#include <format>

namespace utils
{
    std::vector<std::string> Split(const std::string &str, const std::string &sep)
    {
        std::vector<std::string> parts;
        if (str.size() == 0)
        {
            return parts;
        }
        size_t pos = 0, next;
        while ((next = str.find(sep, pos)) != std::string::npos)
        {
            parts.push_back(str.substr(pos, next - pos));
            pos = next + sep.size();
        }
        parts.push_back(str.substr(pos));
        return parts;
    }

    std::expected<int, std::string> SafeSTOI(const std::string &str)
    {
        try
        {
            return std::stoi(str);
        }
        catch (...)
        {
            return std::unexpected(std::format("failed to parse string to int, str={}", str));
        }
    }
}