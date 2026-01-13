#include "tpool/Pool.hpp"
#include "core/HTTPServer.hpp"
#include <functional>

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
    template <typename T, size_t N, size_t M>
    std::function<void()> Pool<T, N, M>::Worker(const std::function<void(T &&item)> &fn)
    {
        return [this, fn]()
        {
            while (!shutdown_.load())
            {
                std::optional<T> _item = Pop();
                if (!_item)
                {
                    continue;
                }
                fn(std::move(*_item));
            }
        };
    }

    template <typename T, size_t N, size_t M>
    Pool<T, N, M>::Pool(const std::function<void(T &&item)> &fn) : queue_(std::array<T, N>()), writePtr_(-1), readPtr_(0), threads_(std::array<std::thread, M>()), shutdown_(false)
    {
        static_assert(N > 0, "size n must be greater than 0");
        static_assert(M > 0, "size m must be greater than 0");
        for (int i = 0; i < M; i++)
        {
            threads_[i] = std::thread(Worker(fn));
        }
    }

    template <typename T, size_t N, size_t M>
    Pool<T, N, M>::~Pool()
    {
        shutdown_.store(true);
        cv_.notify_all();
        for (auto &thread : threads_)
        {
            thread.join();
        }
    }

    template <typename T, size_t N, size_t M>
    std::optional<T> Pool<T, N, M>::Pop()
    {
        std::unique_lock<std::mutex> lock(mtx_);
        cv_.wait(lock, [this]
                 { return (!IsEmpty(readPtr_, writePtr_, N) || shutdown_.load()); });
        if (shutdown_.load())
        {
            return std::nullopt;
        }
        auto data = std::move(queue_[readPtr_]);
        readPtr_ = CalculateIndex(readPtr_ + 1, N);
        return data;
    }

    template <typename T, size_t N, size_t M>
    void Pool<T, N, M>::Push(T &&item)
    {
        std::unique_lock<std::mutex> lock(mtx_);
        writePtr_ = CalculateIndex(writePtr_ + 1, N);
        queue_[writePtr_] = std::move(item);
        cv_.notify_one();
    }

    template <typename T, size_t N, size_t M>
    void Pool<T, N, M>::Submit(T &&item)
    {
        Push(std::move(item));
    }

    template class tpool::Pool<std::unique_ptr<server::HTTPServer<10, 10>::HTTPConnection>, 10, 10>;
}