#include "tpool/Pool.hpp"
#include <functional>

namespace
{
    template <typename T, size_t N>
    std::function<void()> Worker(const std::function<void(const T &item)> &fn, tpool::Queue<T, N> &queue, const std::atomic<bool> &shutdown)
    {
        return [fn, &queue, &shutdown]()
        {
            while (!shutdown.load())
            {
                T item = queue.Pop();
                fn(item);
            }
        };
    }
}

namespace tpool
{
    template <typename T, size_t N, size_t M>
    Pool<T, N, M>::Pool(const std::function<void(const T &item)> &fn) : queue_(Queue<T, N>()), threads_(std::array<std::thread, M>()), fn_(fn), shutdown_(false)
    {
        static_assert(M > 0, "size m must be greater than 0");
        for (int i = 0; i < M; i++)
        {
            threads_[i] = std::thread(Worker(fn_, queue_, shutdown_));
        }
    }

    template <typename T, size_t N, size_t M>
    Pool<T, N, M>::~Pool()
    {
        shutdown_.store(true);
        for (auto &thread : threads_)
        {
            thread.join();
        }
    }

    template <typename T, size_t N, size_t M>
    void Pool<T, N, M>::Submit(const T &item)
    {
        queue_.Push(std::move(item));
    }

    template class Pool<int, 10, 10>;
}