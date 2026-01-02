#include "Server.hpp"
#include <arpa/inet.h>
#include <unistd.h>
#include <format>
#include <ostream>

namespace server
{
    std::ostream &operator<<(std::ostream &os, const Address &addr)
    {
        return os << addr.host << ':' << addr.port;
    }

    std::ostream &operator<<(std::ostream &os, const SocketData &data)
    {
        return os << std::string_view(data.data.data(), data.size);
    }

    namespace connection
    {
        Connection::Connection(int clientDescriptor, Address address) : fd_(clientDescriptor), address_(std::move(address))
        {
        }

        Connection::Connection(Connection &&other)
            : fd_(other.fd_), address_(std::move(other.address_))
        {
            other.fd_ = -1;
        }

        Connection::~Connection()
        {
            close(fd_);
        }

        const Address &Connection::GetAddress() const
        {
            return address_;
        }

        std::expected<std::optional<SocketData>, std::string> Connection::Read()
        {
            SocketData result;
            ssize_t size = read(fd_, &result.data, result.data.size());
            if (size < 0)
            {
                return std::unexpected(std::format("read failed, reason={}", std::strerror(errno)));
            }
            if (size == 0)
            {
                return std::nullopt;
            }
            result.size = static_cast<size_t>(size);
            return result;
        }

        std::expected<bool, std::string> Connection::Write(std::string_view data)
        {
            size_t total = 0;
            while (total < data.size())
            {
                ssize_t n = write(fd_, data.data() + total, data.size() - total);
                if (n <= 0)
                {
                    return std::unexpected(std::format("write failed, reason={}", std::strerror(errno)));
                }
                total += static_cast<size_t>(n);
            }
            return true;
        }

        Connection &Connection::operator=(Connection &&other)
        {
            if (this != &other)
            {
                fd_ = other.fd_;
                address_ = std::move(other.address_);
                other.fd_ = -1;
            }
            return *this;
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
            return std::unexpected(std::format("invalid file descriptor, fd={}", std::strerror(errno)));
        }
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof(ip));
        int port = ntohs(client_addr.sin_port);
        return connection::Connection(clientFD, Address{ip, port});
    }
}