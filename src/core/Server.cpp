#include "core/Server.hpp"
#include <arpa/inet.h>
#include <unistd.h>
#include <format>
#include <ostream>

namespace server
{
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

    Server::Server(Server &&other) : fd_(other.fd_)
    {
        other.fd_ = -1;
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
            return std::unexpected(std::format("invalid file descriptor, fd={}", std::strerror(errno)));
        }
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof(ip));
        int port = ntohs(client_addr.sin_port);
        return connection::Connection(clientFD, Address{ip, port});
    }

    Server &Server::operator=(Server &&other)
    {
        if (this != &other)
        {
            fd_ = other.fd_;
            other.fd_ = -1;
        }
        return *this;
    }
}