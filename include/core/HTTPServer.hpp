#pragma once
#include "Server.hpp"
#include "core/Connection.hpp"
#include "core/HTTPCommon.hpp"
#include "tpool/Pool.hpp"

namespace server
{
    template <size_t N, size_t M>
    class HTTPServer
    {
    private:
        struct Job
        {
        };

        server::Server server_;
        tpool::Pool<Job, N, M> pool_;

        class HTTPConnection
        {
        private:
            Connection connection_;

        public:
            HTTPConnection(Connection &&connection);
            HTTPConnection(const HTTPConnection &other) = delete;
            HTTPConnection(HTTPConnection &&other) = default;
            ~HTTPConnection() = default;
            std::expected<void, std::string> Handle(const common::Handler &handler);
            HTTPConnection &operator=(const HTTPConnection &other) = delete;
            HTTPConnection &operator=(HTTPConnection &&other) = default;
        };

        std::expected<HTTPConnection, std::string> Accept();

    public:
        HTTPServer(Server &&server);
        HTTPServer(const HTTPServer &other) = delete;
        HTTPServer(HTTPServer &&other) = delete;
        ~HTTPServer() = default;
        std::expected<void, std::string> Listen(const common::Handler &handler);
        HTTPServer &operator=(const HTTPServer &other) = delete;
        HTTPServer &operator=(HTTPServer &&other) = delete;
    };
}