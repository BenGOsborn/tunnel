#pragma once
#include <string>
#include <expected>
#include <array>
#include <optional>

constexpr size_t BUFFER_SIZE = 512;

namespace server
{
    struct Address
    {
        std::string host;
        int port;
    };
    std::ostream &operator<<(std::ostream &os, const Address &addr);

    struct SocketData
    {
        std::array<char, BUFFER_SIZE> data;
        size_t size;
    };
    std::ostream &operator<<(std::ostream &os, const SocketData &data);

    namespace connection
    {
        class Connection
        {
        public:
            Connection(int clientDescriptor, Address address);
            Connection(const Connection &other) = delete;
            Connection(Connection &&other);
            ~Connection();
            const Address &GetAddress() const;
            std::expected<std::optional<SocketData>, std::string> Read();
            std::expected<bool, std::string> Write(std::string_view data);
            Connection &operator=(const Connection &other) = delete;
            Connection &operator=(Connection &&other);

        private:
            int fd_;
            Address address_;
        };
    }

    class Server
    {
    public:
        Server(const Address &address);
        Server(const Server &other) = delete;
        Server(Server &&other);
        ~Server();
        std::expected<connection::Connection, std::string> Accept();
        Server &operator=(const Server &other) = delete;
        Server &operator=(Server &&other);

    private:
        int fd_;
    };
}