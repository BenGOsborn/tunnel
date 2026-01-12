#include "core/Server.hpp"
#include <arpa/inet.h>
#include <unistd.h>
#include <format>
#include <ostream>
#include <sys/select.h>
#include <cerrno>

namespace server
{
    Server::Server(const Address &address, int timeout) : timeout_(timeout)
    {
        fd_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(address.port);
        inet_pton(AF_INET, address.host.c_str(), &addr.sin_addr);
        bind(fd_, (sockaddr *)&addr, sizeof(addr));
        listen(fd_, SOMAXCONN);
    }

    Server::Server(Server &&other) : fd_(other.fd_), timeout_(other.timeout_)
    {
        other.fd_ = -1;
    }

    Server::~Server()
    {
        close(fd_);
    }

    std::expected<std::optional<Connection>, std::string> Server::Accept()
    {
        fd_set set;
        FD_ZERO(&set);
        FD_SET(fd_, &set);
        timeval tv{timeout_, 0};
        int ready = select(fd_ + 1, &set, nullptr, nullptr, &tv);
        if (ready < 0)
        {
            return std::unexpected(std::format("client error, err={}", std::strerror(errno)));
        }
        if (ready == 0)
        {
            return std::nullopt;
        }
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
        return Connection(clientFD, Address{ip, port}, timeout_);
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