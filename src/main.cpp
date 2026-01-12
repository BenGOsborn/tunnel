#include "core/HTTPServer.hpp"
#include "handler/Handler.hpp"
#include <exception>
#include <format>
#include <iostream>
#include <csignal>
#include <atomic>

constexpr std::string HOST = "127.0.0.1";
constexpr int PORT = 8080;
constexpr int TIMEOUT_SECS = 5;

std::atomic<bool> shutdown{false};

void HandleShutdown(int)
{
    shutdown.store(true);
}

int main()
{
    std::signal(SIGTERM, HandleShutdown);
    std::signal(SIGINT, HandleShutdown);

    server::HTTPServer<10, 10> httpServer{server::Server{server::Address{HOST, PORT}}, handler::Handle, TIMEOUT_SECS};
    std::cout << "Server is listening... " << HOST << ":" << PORT << std::endl;

    while (!shutdown.load())
    {
        std::expected<void, std::string> _success = httpServer.Listen();
        if (!_success)
        {
            throw std::runtime_error(std::format("failed to accept client, err={}", _success.error()));
        }
    }

    return 0;
}