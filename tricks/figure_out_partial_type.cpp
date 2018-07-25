//
// How does a std::set figure out the partial type of the allocator
// so that it can it can convert the passed in allocator type allocator<T>
// to allocator<treenode<T>> ?
//
// The following is a possible C++11 solution.
//

#include <iostream>
#include <typeinfo>

template<template<typename X> typename T, typename X, typename Y>
T<Y> foo(T<X>, Y) {};

template<typename T>
struct myalloc
{
};


template<typename T>
struct mytreenode
{
};

template<typename T, typename A = myalloc<T>>
struct myset
{
    using alloctype = decltype(
        foo(std::declval<A>(), std::declval<mytreenode<T>>())
    );

    void printalloctype()
    {
        std::cout << typeid(alloctype).name();
    }
};

int main()
{
#ifdef _MSC_VER
    myset<int> s;
    s.printalloctype();
#else
    // compilation failure message due to the following line will show that
    // the type convert is successful.
    typename myset<int>::alloctype a = 1;
#endif
    std::cout << "\ndone";
}
