#include "core/HTTPServer.hpp"
#include "core/HTTPCommon.hpp"
#include "core/Utils.hpp"
#include <format>
#include <iostream>

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

    std::expected<HTTPHeaderKV, std::string> ParseHTTPHeaderKV(const std::string &line)
    {
        std::string delim = ": ";
        size_t pos = line.find(delim);
        if (pos == std::string::npos)
        {
            return std::unexpected("invalid header, missing separator");
        }
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + delim.size());
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
        common::HTTPHeaderKVs headerKVs;
        size_t bodySize = 0;
        for (int i = 1; i < lines.size(); i++)
        {
            const auto &rawHeaderKV = lines[i];
            std::expected<HTTPHeaderKV, std::string> _httpHeaderKV = ParseHTTPHeaderKV(rawHeaderKV);
            if (!_httpHeaderKV)
            {
                return std::unexpected(std::format("failed to parse http header, err={}", _httpHeaderKV.error()));
            }
            auto httpHeaderKV = *_httpHeaderKV;
            headerKVs[httpHeaderKV.key] = httpHeaderKV.value;
            if (httpHeaderKV.key == CONTENT_LENGTH_HEADER)
            {
                std::expected<int, std::string> _bodySize = utils::SafeSTOI(httpHeaderKV.value);
                if (!_bodySize)
                {
                    return std::unexpected(std::format("failed to get body size, err={}", _bodySize.error()));
                }
                bodySize = static_cast<size_t>(*_bodySize);
            }
        }
        return HTTPHeader{pos, bodySize, headerHeader.method, headerHeader.path, headerHeader.version, headerKVs};
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
        common::HTTPHeaderKVs headerKVs = resp.headerKVs;
        headerKVs[CONTENT_LENGTH_HEADER] = std::to_string(resp.body.size());
        for (auto const &[key, val] : headerKVs)
        {
            out += std::format("{}: {}{}", key, val, SEPARATOR);
        }
        out += SEPARATOR;
        out += resp.body;
        return out;
    }

    bool HasMoreHeader(const std::string &req)
    {
        return !req.contains(HEADER_END.c_str());
    }

    bool HasMoreBody(const std::string &req, const HTTPHeader &header)
    {
        return req.size() - header.headerSize < header.bodySize;
    }

    std::expected<void, std::string> SendFailedResponse(server::Connection &connection)
    {
        std::expected<std::string, std::string> _resp = BuildHTTPResponse(common::HTTPResponse{common::HTTPVersion::V1_1, 400, "Bad request", common::HTTPHeaderKVs{}, ""});
        if (!_resp)
        {
            return std::unexpected(std::format("failed to build response, err={}", _resp.error()));
        }
        auto resp = *_resp;
        std::expected<void, std::string> _success = connection.Write(resp);
        if (!_success)
        {
            return std::unexpected(std::format("failed to send response, err={}", _resp.error()));
        }
        return std::expected<void, std::string>{};
    }
}

namespace server
{
    template <size_t N, size_t M>
    HTTPServer<N, M>::HTTPConnection::HTTPConnection(Connection &&connection) : connection_(std::move(connection))
    {
    }

    template <size_t N, size_t M>
    std::expected<void, std::string> HTTPServer<N, M>::HTTPConnection::Handle(const common::Handler &handler)
    {
        while (true)
        {
            std::string req = "";
            while (HasMoreHeader(req))
            {
                std::expected<std::optional<SocketData>, std::string> __data = connection_.Read();
                if (!__data)
                {
                    return std::unexpected(std::format("failed to read header, err={}", __data.error()));
                }
                std::optional<SocketData> _data = *__data;
                if (!_data)
                {
                    return std::expected<void, std::string>{};
                }
                SocketData data = *_data;
                req += std::string(data.data.begin(), data.data.begin() + data.size);
            }
            std::expected<HTTPHeader, std::string> _httpHeader = ParseHTTPHeader(req);
            if (!_httpHeader)
            {
                std::expected<void, std::string> _success = SendFailedResponse(connection_);
                if (!_success)
                {
                    return std::unexpected(std::format("failed to send failed response, err={}", _success.error()));
                }
            }
            auto httpHeader = std::move(*_httpHeader);
            while (HasMoreBody(req, httpHeader))
            {
                std::expected<std::optional<SocketData>, std::string> __data = connection_.Read();
                if (!__data)
                {
                    return std::unexpected(std::format("failed to read body, err={}", __data.error()));
                }
                std::optional<SocketData> _data = *__data;
                if (!_data)
                {
                    return std::expected<void, std::string>{};
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
            std::expected<void, std::string> _success = connection_.Write(resp);
            if (!_success)
            {
                return std::unexpected(std::format("failed to send response, err={}", _resp.error()));
            }
        }
    }

    template <size_t N, size_t M>
    HTTPServer<N, M>::HTTPServer(Server &&server, const common::Handler &handler) : server_(std::move(server)), pool_(tpool::Pool<HTTPServer<N, M>::HTTPConnection, N, M>(Worker(handler)))
    {
    }

    template <size_t N, size_t M>
    std::expected<typename HTTPServer<N, M>::HTTPConnection, std::string> HTTPServer<N, M>::Accept()
    {
        std::expected<Connection, std::string> _conn = server_.Accept();
        if (!_conn)
        {
            return std::unexpected(std::format("failed to get connection, err={}", _conn.error()));
        }
        return HTTPConnection(std::move(*_conn));
    }

    template <size_t N, size_t M>
    std::function<void(typename server::HTTPServer<N, M>::HTTPConnection &&conn)> HTTPServer<N, M>::Worker(const common::Handler &fn)
    {
        return [fn](typename server::HTTPServer<N, M>::HTTPConnection &&conn)
        {
            auto _success = conn.Handle(fn);
            if (!_success)
            {
                std::cout << std::format("failed to handle request, err={}", _success.error()) << std::endl;
            }
        };
    }

    template <size_t N, size_t M>
    std::expected<void, std::string> HTTPServer<N, M>::Listen()
    {
        std::expected<HTTPConnection, std::string> _conn = Accept();
        if (!_conn)
        {
            return std::unexpected(std::format("failed to accept client, err={}", _conn.error()));
        }
        pool_.Submit(std::move(*_conn));
        return std::expected<void, std::string>{};
    }

    template class HTTPServer<10, 10>;
}