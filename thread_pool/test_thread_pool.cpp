#include "thread_pool.h"

#include <iostream>

using namespace utils;

int foo(double f, int i)
{
    return f + i;
}

int main()
{
    thread_pool tp;
    tp.async(foo, 2.3, 7);
    tp.async([]()->void *{std::cout << "yay!"; return nullptr;});
    tp.async([](){std::cout << "woohoo!";});
    std::cout << "\ndone";
}
