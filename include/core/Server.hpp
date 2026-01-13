#pragma once
#include "core/Connection.hpp"
#include <chrono>
#include <optional>

namespace server
{
    class Server
    {
    private:
        int fd_;
        std::chrono::seconds timeout_;

    public:
        Server(const Address &address, std::chrono::seconds timeout);
        Server(const Server &other) = delete;
        Server(Server &&other);
        ~Server();
        std::expected<std::optional<Connection>, std::string> Accept();
        Server &operator=(const Server &other) = delete;
        Server &operator=(Server &&other);
    };
}