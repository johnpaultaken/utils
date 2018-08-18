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
#include <memory>
#include <iostream>

namespace utils
{

/*
Notes:
1.  Use -pthread option with gcc.
2.  Unlike std::async, cannot queue functions with rvalue parameters for now.
*/

class thread_pool
{
public:

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

    void wait()
    {
        while (! q_.empty())
        {
            auto & f = q_.front();
            try
            {
                f();
            }
            catch(...)
            {
                std::cout << "\n Non standard exception in wait()\n";
            }
            q_.pop();
        }
    }
private:
    std::queue<std::function<void()>> q_;
};

} // namespace utils
