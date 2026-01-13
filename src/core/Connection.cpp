#include "core/Connection.hpp"
#include <arpa/inet.h>
#include <unistd.h>

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

    Connection::Connection(int clientDescriptor, Address address, std::chrono::seconds timeout) : fd_(clientDescriptor), address_(std::move(address)), timeout_(timeout)
    {
    }

    Connection::Connection(Connection &&other)
        : fd_(other.fd_), address_(std::move(other.address_)), timeout_(other.timeout_)
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
        fd_set set;
        FD_ZERO(&set);
        FD_SET(fd_, &set);
        timeval tv{timeout_.count(), 0};
        int ready = select(fd_ + 1, &set, nullptr, nullptr, &tv);
        if (ready < 0)
        {
            return std::unexpected(std::format("client error, err={}", std::strerror(errno)));
        }
        if (ready == 0)
        {
            return std::nullopt;
        }
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

    std::expected<void, std::string> Connection::Write(std::string_view data)
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
        return std::expected<void, std::string>{};
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