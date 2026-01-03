#include "Server.hpp"
#include <exception>
#include <format>
#include <iostream>

constexpr std::string HOST = "127.0.0.1";
constexpr int PORT = 8080;

int main()
{
    server::Server server(server::Address{HOST, PORT});

    std::cout << "Server is listening... " << HOST << ":" << PORT << std::endl;

    auto _conn = server.Accept();
    if (!_conn)
    {
        throw std::runtime_error(std::format("failed to accept client, err={}", _conn.error()));
    }
    auto conn = std::move(*_conn);

    auto address = conn.GetAddress();
    std::cout << address << std::endl;

    auto __data = conn.Read();
    if (!__data)
    {
        throw std::runtime_error(std::format("failed to read data, err={}", __data.error()));
    }
    auto _data = *__data;
    if (!_data)
    {
        throw std::runtime_error("data is missing");
    }
    auto data = *_data;
    std::cout << data << std::endl;

    return 0;
}