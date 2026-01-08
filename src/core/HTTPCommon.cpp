#include "core/HTTPCommon.hpp"
#include "core/Utils.hpp"
#include <expected>
#include <vector>
#include <format>
#include <format>

namespace
{
    constexpr std::string SEPARATOR = "\r\n";

    constexpr std::string VERSION_1_1 = "HTTP/1.1";
    constexpr std::string METHOD_GET = "GET";
    constexpr std::string METHOD_POST = "POST";
    constexpr std::string CONTENT_LENGTH_HEADER = "Content-Length";

    struct HTTPRequestLine
    {
        common::HTTPMethod method;
        std::string path;
        common::HTTPVersion version;
    };

    struct HTTPHeader
    {
        std::string key;
        std::string value;
    };

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

    std::expected<HTTPRequestLine, std::string> ParseHTTPRequestLine(const std::string &line)
    {
        std::vector<std::string> split = utils::Split(line, " ");
        if (split.size() != 3)
        {
            return std::unexpected("invalid number of parts for request line");
        }
        auto _method = ParseHTTPMethod(split[0]);
        if (!_method)
        {
            return std::unexpected(std::format("failed to parse method, err={}", _method.error()));
        }
        auto _version = ParseHTTPVersion(split[2]);
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

namespace common
{
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
        return HTTPRequest{requestLine.method, requestLine.path, requestLine.version, httpHeaders, ""};
    }

    std::expected<std::string, std::string> BuildHTTPResponse(const HTTPResponse &resp)
    {
        std::string out = "";
        std::expected<std::string, std::string> _version = SerializeHTTPVersion(resp.version);
        if (!_version)
        {
            return std::unexpected(std::format("failed to serialize http version, err={}", _version.error()));
        }
        auto version = *_version;
        out += std::format("{} {} {}{}", version, resp.statusCode, resp.statusMessage, SEPARATOR);
        HTTPHeaders headers = resp.headers;
        headers[CONTENT_LENGTH_HEADER] = std::to_string(resp.body.size());
        for (auto const &[key, val] : headers)
        {
            out += std::format("{}: {}{}", key, val, SEPARATOR);
        }
        out += SEPARATOR;
        out += resp.body;
        return out;
    }
}