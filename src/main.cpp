#include "core/HTTPServer.hpp"
#include "handler/Handler.hpp"
#include <exception>
#include <format>
#include <iostream>

constexpr std::string HOST = "127.0.0.1";
constexpr int PORT = 8080;

int main()
{
    server::HTTPServer httpServer{server::Server{server::Address{HOST, PORT}}};
    std::cout << "Server is listening... " << HOST << ":" << PORT << std::endl;

    std::expected<void, std::string> _success = httpServer.Listen(handler::Handle);
    if (!_success)
    {
        throw std::runtime_error(std::format("failed to accept client, err={}", _success.error()));
    }

    return 0;
}