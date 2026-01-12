#include "tpool/Queue.hpp"
#include "core/HTTPServer.hpp"

namespace
{
    int CalculateIndex(int idx, size_t size)
    {
        return idx % size;
    }

    bool IsEmpty(int readPtr, int writePtr, size_t size)
    {
        return CalculateIndex(readPtr, size) == CalculateIndex(writePtr + 1, size);
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
    T Queue<T, N>::Pop()
    {
        std::unique_lock<std::mutex> lock(mtx_);
        cv_.wait(lock, [this]
                 { return !IsEmpty(readPtr_, writePtr_, N); });
        auto data = std::move(queue_[readPtr_]);
        readPtr_ = CalculateIndex(readPtr_ + 1, N);
        return data;
    }

    template <typename T, size_t N>
    void Queue<T, N>::Push(T &&item)
    {
        std::unique_lock<std::mutex> lock(mtx_);
        writePtr_ = CalculateIndex(writePtr_ + 1, N);
        queue_[writePtr_] = std::move(item);
        cv_.notify_one();
    }

    template class tpool::Queue<server::HTTPServer<10, 10>::HTTPConnection, 10>;
}