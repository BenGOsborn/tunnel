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
        // **** This is actually not the case because what happens if its in a ring mode...
        if (readPtr_ > writePtr_)
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

    template <typename T, std::size_t N>
    std::expected<void, std::string> Queue<T, N>::Push(const T &data)
    {
    }
}