#include "core/HTTPServer.hpp"
#include "handler/Handler.hpp"
#include <exception>
#include <format>
#include <iostream>
#include <csignal>

constexpr std::string HOST = "127.0.0.1";
constexpr int PORT = 8080;
constexpr int TIMEOUT_SECS = 5;

int main()
{
    server::HTTPServer<10, 10> httpServer{server::Server{server::Address{HOST, PORT}, TIMEOUT_SECS}, handler::Handle};
    std::cout << "Server is listening... " << HOST << ":" << PORT << std::endl;
    std::expected<void, std::string> _success = httpServer.Listen();
    if (!_success)
    {
        throw std::runtime_error(std::format("failed to accept client, err={}", _success.error()));
    }

    return 0;
}