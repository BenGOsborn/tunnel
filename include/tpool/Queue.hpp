#pragma once
#include <array>

namespace tpool
{
    template <typename T, std::size_t N>
    class Queue
    {
        const T &Pop();
        void &Push(const T &data);

    private:
        std::array<T, N> queue_
    };
}