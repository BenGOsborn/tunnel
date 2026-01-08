#include "core/HTTPConnection.hpp"
#include "core/HTTPCommon.hpp"
#include <format>

namespace
{
    std::expected<bool, std::string> SendFailedResponse(server::connection::Connection &connection)
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

namespace server::connection
{
    HTTPConnection::HTTPConnection(Connection &&connection) : connection_(std::move(connection))
    {
    }

    std::expected<bool, std::string> HTTPConnection::Handle(const common::Handler &handler)
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
            std::expected<common::HTTPResponse, std::string> _httpResp = handler(httpReq);
            if (!_httpResp)
            {
                return std::unexpected(std::format("failed to execute handler, err={}", _httpResp.error()));
            }
            std::expected<std::string, std::string> _resp = common::BuildHTTPResponse(*_httpResp);
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
}