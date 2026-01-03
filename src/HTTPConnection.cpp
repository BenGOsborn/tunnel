#include "HTTPConnection.hpp"
#include <format>

#include <iostream>

namespace server::connection
{
    HTTPConnection::HTTPConnection(Connection &&connection) : connection_(std::move(connection))
    {
    }

    std::expected<bool, std::string> HTTPConnection::Handle()
    {
        while (true)
        {
            std::expected<std::optional<SocketData>, std::string> __data = connection_.Read();
            if (!__data)
            {
                return std::unexpected(std::format("failed to read data, err={}", __data.error()));
            }
            std::optional<SocketData> _data = *__data;
            if (!_data)
            {
                return true;
            }
            SocketData data = *_data;

            std::cout << data << std::endl;

            connection_.Write(data.data.data());
        }
    }
}