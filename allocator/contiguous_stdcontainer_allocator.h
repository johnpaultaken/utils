//----------------------------------------------------------------------------
// year   : 2018
// author : John Paul
// email  : johnpaultaken@gmail.com
// source : https://github.com/johnpaultaken
// description :
//      A C++ 11 allocator for containers to put their elements in
//      contiguous memory.
//      Since elements are packed and adjacent to each other, the layout is
//      cache optimized. It allows a fetch of a cache line to contain the
//      maximum number of container elements.
//
//      This allocator is more sophisticated compared to contiguous_allocator
//      because it allows the construction parameter capacity to be the same as
//      the required capacity of the container it is used with. Readme has more
//      details.
//----------------------------------------------------------------------------

#pragma once

#include "contiguous_allocator.h"

// to support safe_capacity specializations.
#include<map>
#include<unordered_map>

namespace utils
{

/*
Notes:
1.  This allocator can be used for cache efficient access of container elements.
2.  It is meant to be used when the container has reached a state when no more
     insert or erase of elements are needed. For example
     a) a dictionary that will be used for only lookup after it is initialized,
     b) a key-value store where only the value changes but not the key.
     Test file has usage examples.
*/

//
// Template Params:
//      Container - the container type this allocator is going to be used for.
//                  Allocator allocation pattern is optimized for different
//                  container types. See readme.
//      T         - the usual allocation type param.
//
template <template <class... Args> class Container, class T>
class _contiguous_stdcontainer_allocator : public contiguous_allocator<T>
{
public:
    template <class... Params>
    using container_type = Container<Params...>;

    //
    // Forward all base class constructors.
    // However param has different semantics.
    // Param: capacity - same value as the intended capacity of the container
    //          this allocator is used for.
    //
    using contiguous_allocator<T>::contiguous_allocator;

    //
    // Containers that need allocation of type other than its template
    // parameter uses this to figure out type of allocator it needs.
    //
    template <typename Type>
    struct rebind
    {
        using other = _contiguous_stdcontainer_allocator<container_type, Type>;
    };
};

//
// Generic version.
//
template <template <class... Args> class Container, class T>
class contiguous_stdcontainer_allocator :
    public _contiguous_stdcontainer_allocator<Container, T>
{
};

//
// Specialized version for std::map.
//
template <class T>
class contiguous_stdcontainer_allocator<std::map, T> :
    public _contiguous_stdcontainer_allocator<std::map, T>
{
public:
    //
    // Params:
    //     capacity - same value as the intended capacity of the container
    //                this allocator is used for.
    //
    contiguous_stdcontainer_allocator(size_t capacity) noexcept :
        _contiguous_stdcontainer_allocator<std::map, T>{
            capacity
    #ifdef _MSC_VER
            + 1     // Visual Studio needs one more for std::map.
    #endif
        }
    {
    }

    contiguous_stdcontainer_allocator(
        const contiguous_stdcontainer_allocator & other
    ) noexcept : _contiguous_stdcontainer_allocator<std::map, T>{other}
    {
    }

    //
    // Must be defined to allow for copy-constructions from allocator objects
    // of other types. See rebind below.
    //
    template <class U>
    contiguous_stdcontainer_allocator(
        const contiguous_stdcontainer_allocator<std::map, U> & other
    ) noexcept : _contiguous_stdcontainer_allocator<std::map, T>{other}
    {
    }
};

//
// Specialized version for std::unordered_map pointer types.
// Used for hash table allocations.
//
template <class T>
class contiguous_stdcontainer_allocator<std::unordered_map, T *> :
    public _contiguous_stdcontainer_allocator<std::unordered_map, T *>
{
};

} // namespace utils

#ifdef UNUSED /////////////////////////////////////////////////////////////////


    pointer allocate(
        size_type n, std::allocator<void>::const_pointer /*hint*/ = 0
    )
    {
        ctrace << "\n->allocate: " << n << " of type " << typeid(T).name()
            << " of size " << sizeof(T);

        if (!pmem_)
        {
            ctrace << "\n\tallocator: " << std::hex << this << std::dec
                << " init for capacity " << capacity_;

            assert(size_ == 0);
            pmem_ = pointer(malloc(capacity_ * sizeof(T)));
        }

        if (size_ + n <= capacity_)
        {
            auto p = pmem_ + size_;
            size_ += n;

            ctrace << "\n\tallocator: " << std::hex << this
                << " return address " << p << std::dec;

            return p;
        }
        else
        {
            ctrace << "\n\tFAIL allocator: " << std::hex << this << std::dec
                << " exceeded capacity." << std::flush;

            throw std::bad_alloc{};
        }
    }

    void deallocate(pointer p, size_type n)
    {
        ctrace << "\n->deallocate: " << n << " of type " << typeid(T).name()
            << " of size " << sizeof(T) << "\n\tat address " << std::hex << p
            << std::dec;

        size_dealloc_ += n;

        // suppress unused parameter warning when TRACE is not defined.
        (void)(p);
    }

    //
    // Compute the safe allocator capacity needed by various container types.
    // Template Param: Container - container type for which computation is for
    // Param: size - desired size of the container.
    // Return: safe allocator capacity to support the provided size.
    //
    template<template<class... Args> class Container>
    static size_t safe_capacity(size_t size)
    {
        return size;
    }

    template<>
    static size_t safe_capacity<std::map>(size_t size)
    {
        return (
            size
#ifdef _MSC_VER
            + 1
#endif
        );
    }

    template<>
    static size_t safe_capacity<std::unordered_map>(size_t size)
    {

        return (
#ifdef _MSC_VER
            capacity_umap_vstudio(size) // Visual Studio
#elif __clang__
            capacity_umap_clang(size) // clang
#else
            size_t(size * 1.1) // gcc
#endif
        );
    }

// private functions
private:

    //
    // Visual studio grows its hash table in the following manner
    // 16      128     1024    2048    4096 ...
    // For more details see readme.txt
    //
    static capacity_umap_vstudio(size_t size)
    {

    }

    //
    // Clang grows its hash table in the following manner
    // 2   5   11  23  47  97  197 397 797 1597 ...
    // For more details see readme.txt
    //
    static capacity_umap_clang(size_t size)
    {

    }

// private data
private:
    size_t capacity_;
    pointer pmem_;
    size_t size_;

    template <class U> friend class contiguous_stdcontainer_allocator;

    // the following members are for sanity check only.
    size_t size_dealloc_;

#endif //UNUSED
