#include "core/HTTPServer.hpp"
#include "core/HTTPCommon.hpp"
#include <format>

namespace
{
    std::expected<bool, std::string> SendFailedResponse(server::Connection &connection)
    {
        std::expected<std::string, std::string> _resp = common::BuildHTTPResponse(common::HTTPResponse{common::HTTPVersion::V1_1, 400, "Bad request", common::HTTPHeaders{}, ""});
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
            while (!req.contains(common::HEADER_END.c_str()))
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
            std::expected<common::HTTPRequest, std::string> _httpReq = common::ParseHTTPRequest(req);
            if (!_httpReq)
            {
                std::expected<bool, std::string> _success = SendFailedResponse(connection_);
                if (!_success)
                {
                    return std::unexpected(std::format("failed to send failed response, err={}", _success.error()));
                }
            }
            auto httpReq = *_httpReq;
            while (req.size() - httpReq.headerSize < httpReq.bodySize)
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
            std::expected<std::string, std::string> _body = common::ParseHTTPBody(req, httpReq.bodySize);
            if (!_body)
            {
                return std::unexpected(std::format("failed to get body, err={}", _body.error()));
            }
            httpReq.body = *_body;
            common::HTTPResponse httpResp = handler(httpReq);
            std::expected<std::string, std::string> _resp = common::BuildHTTPResponse(httpResp);
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