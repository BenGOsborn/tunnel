#pragma once
#include <map>
#include <string>
#include <expected>
#include <functional>

namespace common
{
    using HTTPHeaderKVs = std::map<std::string, std::string>;

    constexpr std::string HEADER_CONTENT_LENGTH = "Content-Length";

    enum HTTPMethod
    {
        Get,
        Post,
    };

    enum HTTPVersion
    {
        V1_1,
    };

    struct HTTPRequest
    {
        HTTPMethod method;
        std::string path;
        HTTPVersion version;
        HTTPHeaderKVs headerKVs;
        std::string body;
    };

    struct HTTPResponse
    {
        HTTPVersion version;
        int statusCode;
        std::string statusMessage;
        HTTPHeaderKVs headerKVs;
        std::string body;
    };

    std::expected<common::HTTPVersion, std::string> ParseHTTPVersion(const std::string &version);
    std::expected<common::HTTPMethod, std::string> ParseHTTPMethod(const std::string &method);
    std::expected<std::string, std::string> SerializeHTTPVersion(const common::HTTPVersion &version);
    std::expected<std::string, std::string> SerializeHTTPMethod(const common::HTTPMethod &method);

    using Handler = std::function<HTTPResponse(const HTTPRequest &req)>;
}