#pragma once
#include "Server.hpp"
#include "core/Connection.hpp"
#include "core/HTTPCommon.hpp"
#include "tpool/Pool.hpp"
#include <functional>
#include <memory>
#include <optional>

namespace server
{
    template <size_t N, size_t M>
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
            std::expected<void, std::string> Handle(const common::Handler &handler);
            HTTPConnection &operator=(const HTTPConnection &other) = delete;
            HTTPConnection &operator=(HTTPConnection &&other) = default;
        };

        tpool::Pool<std::unique_ptr<HTTPConnection>, N, M> pool_;

        std::expected<std::optional<std::unique_ptr<HTTPConnection>>, std::string> Accept();
        std::function<void(typename std::unique_ptr<server::HTTPServer<N, M>::HTTPConnection> conn)> Worker(const common::Handler &fn);

    public:
        HTTPServer(Server &&server, const common::Handler &handler);
        HTTPServer(const HTTPServer &other) = delete;
        HTTPServer(HTTPServer &&other) = delete;
        ~HTTPServer() = default;
        std::expected<void, std::string> Listen();
        HTTPServer &operator=(const HTTPServer &other) = delete;
        HTTPServer &operator=(HTTPServer &&other) = delete;
    };
}