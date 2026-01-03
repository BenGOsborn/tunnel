#pragma once
#include "Connection.hpp"

namespace server::connection
{
    class HTTPConnection
    {
    public:
        HTTPConnection(Connection &&connection);
        HTTPConnection(const HTTPConnection &other) = delete;
        HTTPConnection(HTTPConnection &&other) = default;
        ~HTTPConnection() = default;
        std::expected<bool, std::string> Serve();
        HTTPConnection &operator=(const HTTPConnection &other) = delete;
        HTTPConnection &operator=(HTTPConnection &&other) = default;

    private:
        Connection connection_;
    };
}