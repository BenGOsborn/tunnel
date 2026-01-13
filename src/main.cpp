#include "core/HTTPServer.hpp"
#include "handler/Handler.hpp"
#include <exception>
#include <format>
#include <iostream>
#include <csignal>
#include <atomic>
#include <chrono>

constexpr std::string HOST = "127.0.0.1";
constexpr int PORT = 8080;
constexpr std::chrono::seconds TIMEOUT = std::chrono::seconds(1);
std::atomic<bool> shutdown{false};

void SignalHandler(int)
{
    shutdown.store(true);
}

int main()
{
    std::signal(SIGINT, SignalHandler);
    std::signal(SIGTERM, SignalHandler);
    server::Server svr{TIMEOUT};
    auto _ok = svr.Init(server::Address{HOST, PORT});
    if (!_ok)
    {
        std::cout << std::format("failed to init server, err={}", _ok.error()) << std::endl;
        return -1;
    }
    server::HTTPServer<10, 10> httpServer{std::move(svr), handler::Handle};
    std::cout << "Server is listening... " << HOST << ":" << PORT << std::endl;
    while (!shutdown.load())
    {
        std::expected<void, std::string> _success = httpServer.Listen();
        if (!_success)
        {
            std::cout << std::format("failed to accept client, err={}", _success.error()) << std::endl;
            return -1;
        }
    }
    std::cout << "Exited" << std::endl;
    return 0;
}