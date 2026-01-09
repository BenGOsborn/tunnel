#pragma once
#include <array>
#include <expected>
#include <string>

namespace tpool
{
    template <typename T, std::size_t N>
    class Queue
    {
        Queue();
        ~Queue() = default;
        std::expected<T, std::string> Pop();
        std::expected<void, std::string> Push(const T &data);

    private:
        int writePtr_;
        int readPtr_;
        std::array<T, N> queue_
    };
}