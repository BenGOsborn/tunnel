#include "HTTPServer.hpp"
#include <format>

namespace server
{
    HTTPServer::HTTPServer(Server &&server) : server_(std::move(server))
    {
    }

    std::expected<connection::HTTPConnection, std::string> HTTPServer ::Accept()
    {
        std::expected<connection::Connection, std::string> _conn = server_.Accept();
        if (!_conn)
        {
            return std::unexpected(std::format("failed to get connection, err={}", _conn.error()));
        }
        return connection::HTTPConnection(std::move(*_conn));
    }
}