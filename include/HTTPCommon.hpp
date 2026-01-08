#pragma once
#include <map>
#include <string>
#include <expected>

namespace common
{
    constexpr std::string HEADER_END = "\r\n\r\n";

    using HTTPHeaders = std::map<std::string, std::string>;

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
        std::string body;
    };

    struct HTTPResponse
    {
        HTTPVersion version;
        int statusCode;
        std::string statusMessage;
        HTTPHeaders headers;
        std::string body;
    };

    std::expected<HTTPRequest, std::string> ParseHTTPRequest(const std::string &req);

    std::expected<std::string, std::string> BuildHTTPResponse(const HTTPResponse &resp);
}