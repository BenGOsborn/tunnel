#include "core/HTTPCommon.hpp"
#include "core/Utils.hpp"
#include <expected>
#include <vector>
#include <format>

namespace
{
    constexpr std::string VERSION_1_1 = "HTTP/1.1";
    constexpr std::string METHOD_GET = "GET";
    constexpr std::string METHOD_POST = "POST";
}

namespace common
{
    std::expected<common::HTTPVersion, std::string> ParseHTTPVersion(const std::string &version)
    {
        if (version == VERSION_1_1)
        {
            return common::HTTPVersion::V1_1;
        }
        return std::unexpected(std::format("invalid http version, version={}", version));
    }

    std::expected<common::HTTPMethod, std::string> ParseHTTPMethod(const std::string &method)
    {
        if (method == METHOD_GET)
        {
            return common::HTTPMethod::Get;
        }
        if (method == METHOD_POST)
        {
            return common::HTTPMethod::Post;
        }
        return std::unexpected(std::format("invalid http method, method={}", method));
    }

    std::expected<std::string, std::string> SerializeHTTPVersion(const common::HTTPVersion &version)
    {
        switch (version)
        {
        case common::HTTPVersion::V1_1:
            return VERSION_1_1;
        default:
            return std::unexpected("failed to serialize http version");
        }
    }

    std::expected<std::string, std::string> SerializeHTTPMethod(const common::HTTPMethod &method)
    {
        switch (method)
        {
        case common::HTTPMethod::Get:
            return METHOD_GET;
        case common::HTTPMethod::Post:
            return METHOD_POST;
        default:
            return std::unexpected("failed to serialize http method");
        }
    }
}