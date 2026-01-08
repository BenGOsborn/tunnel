#pragma once
#include <map>
#include <string>
#include <expected>

namespace common
{
    using HTTPHeaders = std::map<std::string, std::string>;

    constexpr std::string HEADER_END = "\r\n\r\n";

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
        HTTPHeaders headers;
    };

    struct HTTPResponse
    {
        HTTPVersion version;
        int statusCode;
        std::string statusMessage;
        HTTPHeaders headers;
        std::string body;
    };

    std::expected<HTTPVersion, std::string> ParseHTTPVersion(const std::string &version);

    std::expected<HTTPMethod, std::string> ParseHTTPMethod(const std::string &method);

    std::expected<HTTPRequest, std::string> ParseHTTPRequest(const std::string &req);

    std::string BuildHTTPResponse(const HTTPResponse &resp);
}