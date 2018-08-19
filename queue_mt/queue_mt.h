//----------------------------------------------------------------------------
// year   : 2018
// author : John Paul
// email  : johnpaultaken@gmail.com
// source : https://github.com/johnpaultaken
// description :
//      Multi-thread safe queue implementation in C++11.
//      Popping thread goes into wait if queue is empty.
//----------------------------------------------------------------------------

#pragma once

#include <queue>
#include <vector>
#include <mutex>
#include <condition_variable>

namespace utils
{
    using std::mutex;
    using std::unique_lock;

    template<typename T>
    class queue_mt
    {
    public:

        //
        // Note: template here allows both rvalue and lvalue parameter
        // without duplicating code for overloaded versions of push.
        //
        template<typename U>
        void push(U && item)
        {
            unique_lock<mutex> l{ mutex_ };
            queue_st_.push(std::forward<U>(item));
            cv_.notify_one();
        }

        template<typename... Args>
        void emplace(Args&&... args)
        {
            unique_lock<mutex> l{ mutex_ };
            queue_st_.emplace(std::forward<Args>(args)...);
            cv_.notify_one();
        }

        T pop()
        {
            unique_lock<mutex> l{mutex_};
            while (queue_st_.empty())
            {
                cv_.wait(l);
            }
            // note: item cannot be auto or reference due to pop that follows.
            T item{ std::move(queue_st_.front()) };
            queue_st_.pop();
            return std::move(item);
        }

        bool empty()
        {
            // a shared lock would do here.
            // However C++11 doesn't have a shared lock mechanism.
            unique_lock<mutex> l{ mutex_ };
            return queue_st_.empty();
        }

        size_t size()
        {
            // a shared lock would do here.
            // However C++11 doesn't have a shared lock mechanism.
            unique_lock<mutex> l{ mutex_ };
            return queue_st_.size();
        }
    private:
        std::condition_variable cv_;
        std::mutex mutex_;
        std::queue<T> queue_st_;
    };
}
