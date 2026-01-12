#pragma once
#include <array>
#include <string>
#include <mutex>
#include <condition_variable>

namespace tpool
{
    template <typename T, size_t N>
    class Queue
    {
    private:
        int writePtr_;
        int readPtr_;
        std::array<T, N> queue_;
        std::mutex mtx_;
        std::condition_variable cv_;

    public:
        Queue();
        ~Queue() = default;
        Queue(const Queue &other) = delete;
        Queue(Queue &&other) = delete;
        T Pop();
        void Push(T &&item);
        Queue &operator=(const Queue &other) = delete;
        Queue &operator=(Queue &&other) = delete;
    };
}