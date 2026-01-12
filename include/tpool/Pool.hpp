#pragma once
#include "Queue.hpp"
#include <array>
#include <thread>
#include <functional>
#include <atomic>

namespace tpool
{
    template <typename T, size_t N, size_t M>
    class Pool
    {
    private:
        Queue<T, N> queue_;
        std::array<std::thread, M> threads_;
        std::function<void(T &&item)> fn_;
        std::atomic<bool> shutdown_;

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