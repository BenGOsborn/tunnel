#pragma once
#include <string>
#include <ostream>
#include <expected>

constexpr size_t BUFFER_SIZE = 512;

namespace server
{
    struct Address
    {
        std::string host;
        int port;
    };
    std::ostream &operator<<(std::ostream &os, const Address &addr);

    namespace connection
    {
        struct SocketData
        {
            std::array<char, BUFFER_SIZE> data;
            size_t size;
        };
        std::ostream &operator<<(std::ostream &os, const SocketData &data);

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
}