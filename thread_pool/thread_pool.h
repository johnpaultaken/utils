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
2.  Since temps are not copied but forwarded as rvalue, temps and literals
    cannot be used as async params.
*/

class thread_pool
{
public:

    template <class Fn, class... Args>
    static typename std::enable_if<!std::is_void<typename std::result_of<Fn(Args...)>::type>::value ,void>::type
    run (
        const std::shared_ptr<std::promise<typename std::result_of<Fn(Args...)>::type>> & ppromise,
        Fn&& fn,
        Args&&... args
    )
    {
        ppromise->set_value(std::forward<Fn>(fn)(std::forward<Args>(args)...));
    }

    template <class Fn, class... Args>
    static typename std::enable_if<std::is_void<typename std::result_of<Fn(Args...)>::type>::value ,void>::type
    run (
        const std::shared_ptr<std::promise<typename std::result_of<Fn(Args...)>::type>> & ppromise,
        Fn&& fn,
        Args&&... args
    )
    {
        std::forward<Fn>(fn)(std::forward<Args>(args)...);
        ppromise->set_value();
    }

    template <class Fn, class... Args>
    std::future<typename std::result_of<Fn(Args...)>::type>
    async (Fn&& fn, Args&&... args)
    {
        auto ppromise = std::make_shared<std::promise<typename std::result_of<Fn(Args...)>::type>>();
        q_.emplace(
            [ppromise, &fn, &args...](){
                try
                {
                    run(ppromise, std::forward<Fn>(fn), std::forward<Args>(args)...);
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
