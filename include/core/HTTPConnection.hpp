#pragma once
#include "core/Connection.hpp"
#include "core/HTTPCommon.hpp"
#include <expected>

namespace server::connection
{
    class HTTPConnection
    {
    public:
        HTTPConnection(Connection &&connection);
        HTTPConnection(const HTTPConnection &other) = delete;
        HTTPConnection(HTTPConnection &&other) = default;
        ~HTTPConnection() = default;
        std::expected<bool, std::string> Handle(const common::Handler &handler);
        HTTPConnection &operator=(const HTTPConnection &other) = delete;
        HTTPConnection &operator=(HTTPConnection &&other) = default;

    private:
        Connection connection_;
    };
}