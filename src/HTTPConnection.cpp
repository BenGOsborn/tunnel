#include "HTTPConnection.hpp"

namespace server::connection
{
    HTTPConnection::HTTPConnection(Connection &&connection) : connection_(std::move(connection))
    {
    }

    std::expected<bool, std::string> HTTPConnection::Serve()
    {
        return false;
    }
}