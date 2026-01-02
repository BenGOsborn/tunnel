#pragma once
#include <string>
#include <expected>

namespace server
{
    struct Address
    {
        std::string host;
        int port;
    };

    namespace connection
    {
        class Connection
        {
        public:
            explicit Connection(int clientDescriptor, Address address);
            ~Connection();
            const Address &GetAddress() const;

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