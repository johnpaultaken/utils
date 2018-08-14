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

namespace utils
{

class thread_pool
{
public:
    template<class T>
    using enable_if_t_void =
        typename std::enable_if<std::is_void<T>::value, T>::type;

    template<class T>
    using enable_if_t_not_void =
        typename std::enable_if<!std::is_void<T>::value, T>::type;

    template <class Fn, class... Args>
    std::future<
        enable_if_t_not_void<typename std::result_of<Fn(Args...)>::type>
    >
    async (Fn&& fn, Args&&... args)
    {
        auto ppromise = std::make_shared<
            std::promise<typename std::result_of<Fn(Args...)>::type>
        >();
        q_.emplace(
            [ppromise, &fn, &args...](){
                try
                {
                    ppromise->set_value(
                        std::forward<Fn>(fn)(std::forward<Args>(args)...)
                    );
                }
                catch(std::exception &)
                {
                    ppromise->set_exception(std::current_exception());
                }
            }
        );
        return ppromise->get_future();
    }

    template <class Fn, class... Args>
    std::future<enable_if_t_void<typename std::result_of<Fn(Args...)>::type>>
    async (Fn&& fn, Args&&... args)
    {
        auto ppromise = std::make_shared<
            std::promise<typename std::result_of<Fn(Args...)>::type>
        >();
        q_.emplace(
            [ppromise, &fn, &args...](){
                try
                {
                    std::forward<Fn>(fn)(std::forward<Args>(args)...);
                    ppromise->set_value();
                }
                catch(std::exception &)
                {
                    ppromise->set_exception(std::current_exception());
                }
            }
        );
        return ppromise->get_future();
    }

private:
    std::queue<std::function<void()>> q_;
};


} // namespace utils
