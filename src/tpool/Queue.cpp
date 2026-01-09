#include "tpool/Queue.hpp"

namespace tpool
{
    template <typename T, std::size_t N>
    Queue<T, N>::Queue() : queue_(std::array<T, N>()), writePtr_(-1), readPtr_(0)
    {
        static_assert(N > 0, "size n must be greater than 0");
    }

    template <typename T, std::size_t N>
    std::expected<T, std::string> Queue<T, N>::Pop()
    {
        if (readPtr_ > writePtr_)
        {
            return std::unexpected("no data to read");
        }
    }

    template <typename T, std::size_t N>
    std::expected<void, std::string> Queue<T, N>::Push(const T &data)
    {
    }
}