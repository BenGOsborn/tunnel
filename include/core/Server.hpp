#pragma once
#include "core/Connection.hpp"

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
        std::expected<Connection, std::string> Accept();
        Server &operator=(const Server &other) = delete;
        Server &operator=(Server &&other);
    };
}