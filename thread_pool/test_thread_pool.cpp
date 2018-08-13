#include "thread_pool.h"

#include <iostream>

int foo(double f, int i)
{
    return f + i;
}

int main()
{
    thread_pool<int> tp;
    tp.async(foo, 2.3, 7);
    tp.async([]()->int{std::cout << "yay!"; return 0;});
    std::cout << "\ndone";
}
