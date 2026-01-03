#include "Server.hpp"
#include "HTTPServer.hpp"
#include <exception>
#include <format>
#include <iostream>

constexpr std::string HOST = "127.0.0.1";
constexpr int PORT = 8080;

using server::connection::HTTPConnection;

int main()
{
    server::HTTPServer httpServer{server::Server{server::Address{HOST, PORT}}};
    std::cout << "Server is listening... " << HOST << ":" << PORT << std::endl;

    std::expected<HTTPConnection, std::string> _conn = httpServer.Accept();
    if (!_conn)
    {
        throw std::runtime_error(std::format("failed to accept client, err={}", _conn.error()));
    }
    HTTPConnection conn = std::move(*_conn);

    auto _success = conn.Handle();
    if (!_success)
    {
        throw std::runtime_error(std::format("failed to read data, err={}", _success.error()));
    }

    return 0;
}