#pragma once
#include <string>
#include <expected>
#include <array>

constexpr size_t BUFFER_SIZE = 512;

namespace server
{
    struct Address
    {
        std::string host;
        int port;
    };

    struct SocketData
    {
        std::array<char, BUFFER_SIZE> data;
        size_t size;
    };

    namespace connection
    {
        class Connection
        {
        public:
            explicit Connection(int clientDescriptor, Address address);
            ~Connection();
            const Address &GetAddress() const;
            std::expected<std::optional<SocketData>, std::string> Read();
            std::expected<bool, std::string> Write(std::string_view data);

        private:
            int fd_;
            Address address_;
        };
    }

    class Server
    {
    public:
        explicit Server(const Address &address);
        ~Server();
        std::expected<connection::Connection, std::string> Accept();

    private:
        int fd_;
    };
}