#include "HTTPRequest.hpp"
#include "Utils.hpp"
#include <expected>
#include <format>

namespace
{
    struct HTTPRequestLine
    {
        request::HTTPMethod method;
        std::string path;
        request::HTTPVersion version;
    };

    struct HTTPHeader
    {
        std::string key;
        std::string value;
    };

    std::expected<HTTPRequestLine, std::string> ParseHTTPRequestLine(const std::string &line)
    {
        std::vector<std::string> split = utils::Split(line, " ");
        if (split.size() != 3)
        {
            return std::unexpected("invalid number of parts for request line");
        }
        auto _method = request::ParseHTTPMethod(split[0]);
        if (!_method)
        {
            return std::unexpected(std::format("failed to parse method, err={}", _method.error()));
        }
        auto _version = request::ParseHTTPVersion(split[2]);
        if (!_version)
        {
            return std::unexpected(std::format("failed to parse method, err={}", _method.error()));
        }
        return HTTPRequestLine{*_method, split[1], *_version};
    }

    std::expected<HTTPHeader, std::string> ParseHTTPHeader(const std::string &header)
    {
        size_t pos = header.find(": ");
        if (pos == std::string::npos)
        {
            return std::unexpected("invalid header, missing separator");
        }
        std::string key = header.substr(0, pos);
        std::string value = header.substr(pos + 2);
        return HTTPHeader{key, value};
    }
}

namespace request
{
    std::expected<HTTPVersion, std::string> ParseHTTPVersion(const std::string &version)
    {
        if (version == "HTTP/1.1")
        {
            return HTTPVersion::V1_1;
        }
        return std::unexpected(std::format("invalid http version, version={}", version));
    }

    std::expected<HTTPMethod, std::string> ParseHTTPMethod(const std::string &method)
    {
        if (method == "GET")
        {
            return HTTPMethod::Get;
        }
        if (method == "POST")
        {
            return HTTPMethod::Post;
        }
        return std::unexpected(std::format("invalid http method, method={}", method));
    }

    std::expected<HTTPRequest, std::string> ParseHTTPRequest(const std::string &req)
    {
        size_t pos = req.find(HEADER_END);
        if (pos == std::string::npos)
        {
            return std::unexpected("unable to find the end of the header");
        }
        std::vector<std::string> lines = utils::Split(req.substr(0, pos), "\r\n");
        if (lines.size() == 0)
        {
            return std::unexpected("invalid number of lines");
        }
        auto _requestLine = ParseHTTPRequestLine(lines[0]);
        if (!_requestLine)
        {
            return std::unexpected(std::format("failed to parse request line, err={}", _requestLine.error()));
        }
        auto requestLine = *_requestLine;
        HTTPHeaders httpHeaders;
        for (int i = 1; i < lines.size(); i++)
        {
            const auto &header = lines[i];
            std::expected<HTTPHeader, std::string> _httpHeader = ParseHTTPHeader(header);
            if (!_httpHeader)
            {
                return std::unexpected(std::format("failed to parse http header, err={}", _httpHeader.error()));
            }
            auto httpHeader = *_httpHeader;
            httpHeaders[httpHeader.key] = httpHeader.value;
        }
        return HTTPRequest{requestLine.method, requestLine.path, requestLine.version, httpHeaders};
    }
}