#include "Server.hpp"
#include <arpa/inet.h>
#include <unistd.h>
#include <format>

namespace server
{
    namespace connection
    {
        Connection::Connection(int clientDescriptor, Address address) : fd_(clientDescriptor), address_(std::move(address))
        {
        }

        Connection::~Connection()
        {
            close(fd_);
        }

        const Address &Connection::GetAddress() const
        {
            return address_;
        }
    }

    Server::Server(const Address &address)
    {
        fd_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(address.port);
        inet_pton(AF_INET, address.host.c_str(), &addr.sin_addr);
        bind(fd_, (sockaddr *)&addr, sizeof(addr));
        listen(fd_, SOMAXCONN);
    }

    Server::~Server()
    {
        close(fd_);
    }

    std::expected<connection::Connection, std::string> Server::Accept()
    {
        sockaddr_in client_addr;
        socklen_t len = sizeof(client_addr);
        int clientFD = accept(fd_, (sockaddr *)&client_addr, &len);
        if (clientFD < 0)
        {
            return std::unexpected(std::format("invalid file descriptor, fd={}", clientFD));
        }
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof(ip));
        int port = ntohs(client_addr.sin_port);
        return connection::Connection(clientFD, Address{ip, port});
    }
}