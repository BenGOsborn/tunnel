#pragma once
#include "core/Connection.hpp"
#include <optional>

namespace server
{
    class Server
    {
    private:
        int fd_;

    public:
        Server(const Address &address);
        Server(const Server &other) = delete;
        Server(Server &&other);
        ~Server();
        std::expected<std::optional<Connection>, std::string> Accept(int timeout);
        Server &operator=(const Server &other) = delete;
        Server &operator=(Server &&other);
    };
}