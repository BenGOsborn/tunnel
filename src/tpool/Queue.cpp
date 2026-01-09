#include "tpool/Queue.hpp"

namespace
{
    int CalculateIndex(int idx, size_t size)
    {
        return idx % size;
    }
}

namespace tpool
{
    template <typename T, size_t N>
    Queue<T, N>::Queue() : queue_(std::array<T, N>()), writePtr_(-1), readPtr_(0)
    {
        static_assert(N > 0, "size n must be greater than 0");
    }

    template <typename T, size_t N>
    std::expected<T, std::string> Queue<T, N>::Pop()
    {
        if (CalculateIndex(readPtr_ + 1) == CalculateIndex(writePtr_))
        {
            return std::unexpected("no data to read");
        }
        auto data = queue_[readPtr_];
        readPtr_ += 1;
        if (readPtr_ >= queue_.size())
        {
            readPtr_ = 0;
        }
    }

    template <typename T, size_t N>
    std::expected<void, std::string> Queue<T, N>::Push(const T &data)
    {
    }
}