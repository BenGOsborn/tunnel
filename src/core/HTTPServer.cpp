#include "core/HTTPServer.hpp"
#include "core/HTTPCommon.hpp"
#include "core/Utils.hpp"
#include <format>

namespace
{
    constexpr std::string HEADER_END = "\r\n\r\n";
    constexpr std::string SEPARATOR = "\r\n";

    constexpr std::string CONTENT_LENGTH_HEADER = "Content-Length";

    struct HTTPHeaderHeader
    {
        common::HTTPMethod method;
        std::string path;
        common::HTTPVersion version;
    };

    struct HTTPHeaderKV
    {
        std::string key;
        std::string value;
    };

    struct HTTPHeader
    {
        size_t headerSize;
        size_t bodySize;
        common::HTTPMethod method;
        std::string path;
        common::HTTPVersion version;
        common::HTTPHeaderKVs headers;
    };

    std::expected<HTTPHeaderHeader, std::string> ParseHTTPHeaderHeader(const std::string &line)
    {
        std::vector<std::string> split = utils::Split(line, " ");
        if (split.size() != 3)
        {
            return std::unexpected("invalid number of parts for request line");
        }
        auto _method = common::ParseHTTPMethod(split[0]);
        if (!_method)
        {
            return std::unexpected(std::format("failed to parse method, err={}", _method.error()));
        }
        auto _version = common::ParseHTTPVersion(split[2]);
        if (!_version)
        {
            return std::unexpected(std::format("failed to parse method, err={}", _method.error()));
        }
        return HTTPHeaderHeader{*_method, split[1], *_version};
    }

    std::expected<HTTPHeaderKV, std::string> ParseHTTPHeaderKV(const std::string &header)
    {
        size_t pos = header.find(": ");
        if (pos == std::string::npos)
        {
            return std::unexpected("invalid header, missing separator");
        }
        std::string key = header.substr(0, pos);
        std::string value = header.substr(pos + 2);
        return HTTPHeaderKV{key, value};
    }

    std::expected<HTTPHeader, std::string> ParseHTTPHeader(const std::string &req)
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
        auto _headerHeader = ParseHTTPHeaderHeader(lines[0]);
        if (!_headerHeader)
        {
            return std::unexpected(std::format("failed to parse header header, err={}", _headerHeader.error()));
        }
        auto headerHeader = *_headerHeader;
        common::HTTPHeaderKVs headers;
        size_t bodySize = 0;
        for (int i = 1; i < lines.size(); i++)
        {
            const auto &header = lines[i];
            std::expected<HTTPHeaderKV, std::string> _httpHeader = ParseHTTPHeaderKV(header);
            if (!_httpHeader)
            {
                return std::unexpected(std::format("failed to parse http header, err={}", _httpHeader.error()));
            }
            auto httpHeader = *_httpHeader;
            headers[httpHeader.key] = httpHeader.value;
            if (httpHeader.key == CONTENT_LENGTH_HEADER)
            {
                std::expected<int, std::string> _bodySize = utils::SafeSTOI(httpHeader.value);
                if (!_bodySize)
                {
                    return std::unexpected(std::format("failed to get body size, err={}", _bodySize.error()));
                }
                bodySize = static_cast<size_t>(*_bodySize);
            }
        }
        return HTTPHeader{pos, bodySize, headerHeader.method, headerHeader.path, headerHeader.version, headers};
    }

    std::expected<std::string, std::string> ParseHTTPBody(const std::string &req, size_t bodySize)
    {
        if (bodySize == 0)
        {
            return "";
        }
        size_t pos = req.find(HEADER_END);
        if (pos == std::string::npos)
        {
            return std::unexpected("unable to find the end of the header");
        }
        size_t offset = pos + HEADER_END.size();
        return req.substr(offset, offset + bodySize);
    }

    std::expected<std::string, std::string> BuildHTTPResponse(const common::HTTPResponse &resp)
    {
        std::string out = "";
        std::expected<std::string, std::string> _version = common::SerializeHTTPVersion(resp.version);
        if (!_version)
        {
            return std::unexpected(std::format("failed to serialize http version, err={}", _version.error()));
        }
        auto version = *_version;
        out += std::format("{} {} {}{}", version, resp.statusCode, resp.statusMessage, SEPARATOR);
        common::HTTPHeaderKVs headers = resp.headers;
        headers[CONTENT_LENGTH_HEADER] = std::to_string(resp.body.size());
        for (auto const &[key, val] : headers)
        {
            out += std::format("{}: {}{}", key, val, SEPARATOR);
        }
        out += SEPARATOR;
        out += resp.body;
        return out;
    }

    std::expected<bool, std::string> SendFailedResponse(server::Connection &connection)
    {
        std::expected<std::string, std::string> _resp = BuildHTTPResponse(common::HTTPResponse{common::HTTPVersion::V1_1, 400, "Bad request", common::HTTPHeaderKVs{}, ""});
        if (!_resp)
        {
            return std::unexpected(std::format("failed to build response, err={}", _resp.error()));
        }
        auto resp = *_resp;
        std::expected<bool, std::string> _success = connection.Write(resp);
        if (!_success)
        {
            return std::unexpected(std::format("failed to send response, err={}", _resp.error()));
        }
        return true;
    }
}

namespace server
{
    HTTPServer::HTTPConnection::HTTPConnection(Connection &&connection) : connection_(std::move(connection))
    {
    }

    std::expected<bool, std::string> HTTPServer::HTTPConnection::Handle(const common::Handler &handler)
    {
        while (true)
        {
            std::string req = "";
            while (!req.contains(HEADER_END.c_str()))
            {
                std::expected<std::optional<SocketData>, std::string> __data = connection_.Read();
                if (!__data)
                {
                    return std::unexpected(std::format("failed to read header, err={}", __data.error()));
                }
                std::optional<SocketData> _data = *__data;
                if (!_data)
                {
                    return true;
                }
                SocketData data = *_data;
                req += std::string(data.data.begin(), data.data.begin() + data.size);
            }
            std::expected<HTTPHeader, std::string> _httpHeader = ParseHTTPHeader(req);
            if (!_httpHeader)
            {
                std::expected<bool, std::string> _success = SendFailedResponse(connection_);
                if (!_success)
                {
                    return std::unexpected(std::format("failed to send failed response, err={}", _success.error()));
                }
            }
            auto httpHeader = std::move(*_httpHeader);
            while (req.size() - httpHeader.headerSize < httpHeader.bodySize)
            {
                std::expected<std::optional<SocketData>, std::string> __data = connection_.Read();
                if (!__data)
                {
                    return std::unexpected(std::format("failed to read body, err={}", __data.error()));
                }
                std::optional<SocketData> _data = *__data;
                if (!_data)
                {
                    return true;
                }
                SocketData data = *_data;
                req += std::string(data.data.begin(), data.data.begin() + data.size);
            }
            std::expected<std::string, std::string> _body = ParseHTTPBody(req, httpHeader.bodySize);
            if (!_body)
            {
                return std::unexpected(std::format("failed to get body, err={}", _body.error()));
            }
            common::HTTPResponse httpResp = handler(common::HTTPRequest{httpHeader.method, httpHeader.path, httpHeader.version, httpHeader.headers, std::move(*_body)});
            std::expected<std::string, std::string> _resp = BuildHTTPResponse(httpResp);
            if (!_resp)
            {
                return std::unexpected(std::format("failed to build response, err={}", _resp.error()));
            }
            auto resp = *_resp;
            std::expected<bool, std::string> _success = connection_.Write(resp);
            if (!_success)
            {
                return std::unexpected(std::format("failed to send response, err={}", _resp.error()));
            }
        }
    }

    HTTPServer::HTTPServer(Server &&server) : server_(std::move(server))
    {
    }

    std::expected<HTTPServer::HTTPConnection, std::string> HTTPServer::Accept()
    {
        std::expected<Connection, std::string> _conn = server_.Accept();
        if (!_conn)
        {
            return std::unexpected(std::format("failed to get connection, err={}", _conn.error()));
        }
        return HTTPConnection(std::move(*_conn));
    }

    std::expected<bool, std::string> HTTPServer::Listen(const common::Handler &handler)
    {
        while (true)
        {
            std::expected<HTTPConnection, std::string> _conn = Accept();
            if (!_conn)
            {
                return std::unexpected(std::format("failed to accept client, err={}", _conn.error()));
            }
            HTTPConnection conn = std::move(*_conn);
            auto _success = conn.Handle(handler);
            if (!_success)
            {
                throw std::unexpected(std::format("failed to handle request, err={}", _success.error()));
            }
        }
    }
}