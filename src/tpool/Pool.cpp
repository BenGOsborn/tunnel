#include "tpool/Pool.hpp"

namespace tpool
{
    template <typename T, size_t N, size_t M>
    Pool<T, N, M>::Pool(const std::function<void(const T &item)> &fn) : queue_(Queue<T, N>()), threads_(std::array<std::thread, M>()), fn_(fn)
    {
    }

    template <typename T, size_t N, size_t M>
    void Pool<T, N, M>::Submit(const T &item)
    {
    }

    template <typename T, size_t N, size_t M>
    void Pool<T, N, M>::Start()
    {
    }
}