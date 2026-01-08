#pragma once
#include "core/Connection.hpp"

namespace server
{
    class Server
    {
    public:
        Server(const Address &address);
        Server(const Server &other) = delete;
        Server(Server &&other);
        ~Server();
        std::expected<connection::Connection, std::string> Accept();
        Server &operator=(const Server &other) = delete;
        Server &operator=(Server &&other);

    private:
        int fd_;
    };
}