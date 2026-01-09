#pragma once
#include "Server.hpp"
#include "core/Connection.hpp"
#include "core/HTTPCommon.hpp"

namespace server
{
    class HTTPServer
    {
    private:
        server::Server server_;

        class HTTPConnection
        {
        private:
            Connection connection_;

        public:
            HTTPConnection(Connection &&connection);
            HTTPConnection(const HTTPConnection &other) = delete;
            HTTPConnection(HTTPConnection &&other) = default;
            ~HTTPConnection() = default;
            std::expected<bool, std::string> Handle(const common::Handler &handler);
            HTTPConnection &operator=(const HTTPConnection &other) = delete;
            HTTPConnection &operator=(HTTPConnection &&other) = default;
        };

        std::expected<HTTPConnection, std::string> Accept();

    public:
        HTTPServer(Server &&server);
        HTTPServer(const HTTPServer &other) = delete;
        HTTPServer(HTTPServer &&other) = default;
        ~HTTPServer() = default;
        std::expected<bool, std::string> Listen(const common::Handler &handler);
        HTTPServer &operator=(const HTTPServer &other) = delete;
        HTTPServer &operator=(HTTPServer &&other) = default;
    };
}