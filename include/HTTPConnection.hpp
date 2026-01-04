#pragma once
#include "Connection.hpp"
#include <map>
#include <string>

namespace server::connection
{
    using Headers = std::map<std::string, std::string>;

    enum HTTPMethod
    {
        Get,
        Post,
    };

    enum HTTPVersion
    {
        V1_1,
    };

    struct HTTPRequest
    {
        HTTPMethod method;
        std::string path;
        HTTPVersion version;
        Headers headers;
    };

    class HTTPConnection
    {
    public:
        HTTPConnection(Connection &&connection);
        HTTPConnection(const HTTPConnection &other) = delete;
        HTTPConnection(HTTPConnection &&other) = default;
        ~HTTPConnection() = default;
        std::expected<bool, std::string> Handle();
        HTTPConnection &operator=(const HTTPConnection &other) = delete;
        HTTPConnection &operator=(HTTPConnection &&other) = default;

    private:
        Connection connection_;
    };
}