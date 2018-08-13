//----------------------------------------------------------------------------
// year   : 2018
// author : John Paul
// email  : johnpaultaken@gmail.com
// source : https://github.com/johnpaultaken
// description :
//      A thread-pool implementation in C++11.
//      Similar interface as the C++11 async task system with the difference
//      of allowing you to specify the number of threads in the pool.
//----------------------------------------------------------------------------

#pragma once

#include <future>
#include <functional>
#include <queue>

template <class ReturnType>
class thread_pool
{
public:
    template <class Fn, class... Args>
    void //std::future<ReturnType>
    async (Fn&& fn, Args&&... args)
    {
        q_.emplace(
            std::bind(std::forward<Fn>(fn), std::forward<Args>(args)...)
        );
    }

private:
    std::queue<std::function<ReturnType()>> q_;
};
