#include "HTTPConnection.hpp"
#include "Utils.hpp"
#include <format>

#include <iostream>

static const std::string HEADER_END = "\r\n\r\n";

namespace
{
    struct HTTPRequestLine
    {
        server::connection::HTTPMethod method;
        std::string path;
        server::connection::HTTPVersion version;
    };

    struct HTTPHeader
    {
        std::string key;
        std::string value;
    };

    std::expected<server::connection::HTTPVersion, std::string>
    ParseHTTPVersion(const std::string &version)
    {
        if (version == "HTTP/1.1")
        {
            return server::connection::HTTPVersion::V1_1;
        }
        return std::unexpected(std::format("invalid http version, version={}", version));
    }

    std::expected<server::connection::HTTPMethod, std::string> ParseHTTPMethod(const std::string &method)
    {
        if (method == "GET")
        {
            return server::connection::HTTPMethod::Get;
        }
        if (method == "POST")
        {
            return server::connection::HTTPMethod::Post;
        }
        return std::unexpected(std::format("invalid http method, method={}", method));
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

    std::expected<server::connection::HTTPRequest, std::string> ParseHTTPRequest(const std::string &req)
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
        server::connection::Headers headers;
        for (int i = 1; i < lines.size(); i++)
        {
            const auto &header = lines[i];
            std::expected<HTTPHeader, std::string> _httpHeader = ParseHTTPHeader(header);
            if (!_httpHeader)
            {
                return std::unexpected(std::format("failed to parse http header, err={}", _httpHeader.error()));
            }
            auto httpHeader = *_httpHeader;
            headers[httpHeader.key] = httpHeader.value;
        }
        return server::connection::HTTPRequest{requestLine.method, requestLine.path, requestLine.version, headers};
    }
}

namespace server::connection
{
    HTTPConnection::HTTPConnection(Connection &&connection) : connection_(std::move(connection))
    {
    }

    std::expected<bool, std::string> HTTPConnection::Handle()
    {
        while (true)
        {
            std::string req = "";
            while (!req.contains(HEADER_END.c_str()))
            {
                std::expected<std::optional<SocketData>, std::string> __data = connection_.Read();
                if (!__data)
                {
                    return std::unexpected(std::format("failed to read data, err={}", __data.error()));
                }
                std::optional<SocketData> _data = *__data;
                if (!_data)
                {
                    return true;
                }
                SocketData data = *_data;
                for (int i = 0; i < data.size; i++)
                {
                    req += std::string(1, data.data[i]);
                }
            }
            std::expected<HTTPRequest, std::string> _httpReq = ParseHTTPRequest(req);
            if (!_httpReq)
            {
                std::cout << "We need to return a 400 here" << std::endl;
            }
            std::cout << (*_httpReq).path << std::endl;
        }
    }
}