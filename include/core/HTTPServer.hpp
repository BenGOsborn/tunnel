#pragma once
#include "Server.hpp"
#include "core/HTTPConnection.hpp"

namespace server
{
    class HTTPServer
    {
    public:
        HTTPServer(Server &&server);
        HTTPServer(const HTTPServer &other) = delete;
        HTTPServer(HTTPServer &&other) = default;
        ~HTTPServer() = default;
        std::expected<connection::HTTPConnection, std::string> Accept();
        HTTPServer &operator=(const HTTPServer &other) = delete;
        HTTPServer &operator=(HTTPServer &&other) = default;

    private:
        server::Server server_;
    };
}