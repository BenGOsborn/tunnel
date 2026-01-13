#pragma once
#include "core/Connection.hpp"
#include <chrono>
#include <optional>
#include <expected>
#include <string>

namespace server
{
    class Server
    {
    private:
        int fd_;
        std::chrono::seconds timeout_;

    public:
        Server(std::chrono::seconds timeout);
        Server(const Server &other) = delete;
        Server(Server &&other);
        ~Server();
        std::expected<void, std::string> Init(const Address &address);
        std::expected<std::optional<Connection>, std::string> Accept();
        Server &operator=(const Server &other) = delete;
        Server &operator=(Server &&other);
    };
}