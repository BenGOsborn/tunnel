#pragma once
#include "Queue.hpp"
#include <array>
#include <thread>
#include <functional>

namespace tpool
{
    template <typename T, size_t N, size_t M>
    class Pool
    {
    private:
        Queue<T, N> queue_;
        std::array<std::thread, M> threads_;
        std::function<void(const T &item)> fn_;

    public:
        Pool(const std::function<void(const T &item)> &fn);
        void Submit(const T &item);
        void Start();
    };
}