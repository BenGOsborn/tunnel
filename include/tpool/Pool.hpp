#pragma once
#include <array>
#include <thread>
#include <functional>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <optional>

namespace tpool
{
    template <typename T, size_t N, size_t M>
    class Pool
    {
    private:
        std::array<T, N> queue_;
        int writePtr_;
        int readPtr_;
        std::mutex mtx_;
        std::condition_variable cv_;
        std::array<std::thread, M> threads_;
        std::atomic<bool> shutdown_;

        std::function<void()> Worker(const std::function<void(T &&item)> &fn);
        std::optional<T> Pop();
        void Push(T &&item);

    public:
        Pool(const std::function<void(T &&item)> &fn);
        ~Pool();
        Pool(const Pool &other) = delete;
        Pool(Pool &&other) = delete;
        void Submit(T &&item);
        Pool &operator=(const Pool &other) = delete;
        Pool &operator=(Pool &&other) = delete;
    };
}