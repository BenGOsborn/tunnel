#pragma once
#include <vector>
#include <string>
#include <expected>

namespace utils
{
    std::vector<std::string> Split(const std::string &str, const std::string &sep);

    std::expected<int, std::string> SafeSTOI(const std::string &str);
}