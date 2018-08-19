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
#include <memory>
#include <iostream>
#include <thread>
#include <vector>

#include "../queue_mt/queue_mt.h"

namespace utils
{

/*
Notes:
1.  Use -pthread option with gcc and clang.
2.  Unlike std::async, cannot queue functions with rvalue parameters for now.
*/

class thread_pool
{
public:
    //
    // num_threads : number of threads requested in the pool.
    //
    thread_pool (size_t num_threads = std::thread::hardware_concurrency()) :
        quit_{false}
    {
        num_threads = std::max(num_threads, size_t{1});
        threads_.reserve(num_threads);
        for(size_t i=0; i<num_threads; ++i)
        {
            threads_.emplace_back(&thread_pool::thread_func, this);
        }
        std::cout << "\nthread_pool: started " << num_threads << " threads.";
    }

    template <class Fn, class... Args>
    std::future<typename std::result_of<Fn(Args...)>::type>
    async (Fn&& fn, Args&&... args)
    {
        auto ppromise = std::make_shared<
            std::promise<typename std::result_of<Fn(Args...)>::type>
        >();
        auto fn_argsbound = std::bind(
            std::forward<Fn>(fn), std::forward<Args>(args)...
        );
        q_.emplace(
            [ppromise, fn_argsbound](){
                try
                {
                    run(ppromise, fn_argsbound);
                }
                catch(std::exception &)
                {
                    ppromise->set_exception(std::current_exception());
                }
            }
        );
        return ppromise->get_future();
    }

    void join()
    {
        for(size_t i=0; i<threads_.size(); ++i)
        {
            q_.emplace([this](){quit_=true;});
        }

        for(auto & thread : threads_)
        {
            thread.join();
        }

        threads_.clear();
    }

    ~thread_pool()
    {
        join();
    }

    // No copy construction or assignment.
    thread_pool(const thread_pool &) = delete;
    thread_pool(thread_pool &&) = delete;
    thread_pool & operator=(const thread_pool &) = delete;
    thread_pool & operator=(thread_pool &&) = delete;

private:    // private member functions

    void thread_func()
    {
        while (! quit_)
        {
            auto f = q_.pop();
            try
            {
                f();
            }
            catch(...)
            {
                std::cout   << "\n Non standard exception in "
                            << f.target_type().name() << "\n";
            }
        }
    }

    template <class Fn>
    static typename std::enable_if<
        !std::is_void<typename std::result_of<Fn()>::type>::value ,void
    >::type
    run (
        const std::shared_ptr<
            std::promise<typename std::result_of<Fn()>::type>
        > & ppromise,
        Fn && fn
    )
    {
        ppromise->set_value(std::forward<Fn>(fn)());
    }

    template <class Fn>
    static typename std::enable_if<
        std::is_void<typename std::result_of<Fn()>::type>::value ,void
    >::type
    run (
        const std::shared_ptr<
            std::promise<typename std::result_of<Fn()>::type>
        > & ppromise,
        Fn && fn
    )
    {
        std::forward<Fn>(fn)();
        ppromise->set_value();
    }

private:    // private data members

    // threads in the pool
    std::vector<std::thread> threads_;

    // the task queue
    queue_mt<std::function<void()>> q_;

    // quit signal
    std::atomic<bool> quit_;
};

} // namespace utils
