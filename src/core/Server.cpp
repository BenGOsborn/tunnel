#include "core/Server.hpp"
#include <arpa/inet.h>
#include <unistd.h>
#include <format>
#include <ostream>
#include <sys/select.h>
#include <cerrno>

namespace server
{
    Server::Server(std::chrono::seconds timeout) : fd_(-1), timeout_(timeout)
    {
    }

    Server::Server(Server &&other) : fd_(other.fd_), timeout_(other.timeout_)
    {
        other.fd_ = -1;
    }

    Server::~Server()
    {
        if (fd_ >= 0)
        {
            close(fd_);
        }
    }

    std::expected<void, std::string> Server::Init(const Address &address)
    {
        fd_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (fd_ < 0)
        {
            return std::unexpected(std::format("socket init err, err={}", std::strerror(errno)));
        }
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(address.port);
        if (inet_pton(AF_INET, address.host.c_str(), &addr.sin_addr) < 0)
        {
            return std::unexpected(std::format("inet pton err, err={}", std::strerror(errno)));
        }
        int opt = 1;
        if (setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        {
            return std::unexpected(std::format("set sock opt err, err={}", std::strerror(errno)));
        }
        if (bind(fd_, (sockaddr *)&addr, sizeof(addr)) < 0)
        {
            return std::unexpected(std::format("bind err, err={}", std::strerror(errno)));
        }
        if (listen(fd_, SOMAXCONN) < 0)
        {
            return std::unexpected(std::format("listen err, err={}", std::strerror(errno)));
        }
        return std::expected<void, std::string>();
    }

    std::expected<std::optional<Connection>, std::string> Server::Accept()
    {
        fd_set set;
        FD_ZERO(&set);
        FD_SET(fd_, &set);
        timeval tv{timeout_.count(), 0};
        int ready = select(fd_ + 1, &set, nullptr, nullptr, &tv);
        if (ready < 0)
        {
            return std::unexpected(std::format("select error, err={}", std::strerror(errno)));
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
            return std::unexpected(std::format("accept err, err={}", std::strerror(errno)));
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