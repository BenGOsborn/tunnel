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

    auto conn_ = server.Accept();
    if (!conn_)
    {
        throw std::runtime_error(std::format("failed to accept client, err={}", conn_.error()));
    }
    auto conn = std::move(*conn_);

    auto address = conn.GetAddress();
    std::cout << address << std::endl;

    auto data__ = conn.Read();
    if (!data__)
    {
        throw std::runtime_error(std::format("failed to read data, err={}", data__.error()));
    }
    auto data_ = *data__;
    if (!data_)
    {
        throw std::runtime_error("data is missing");
    }
    auto data = *data_;
    std::cout << data << std::endl;

    return 0;
}