//----------------------------------------------------------------------------
// year   : 2018
// author : John Paul
// email  : johnpaultaken@gmail.com
// source : https://github.com/johnpaultaken
// description :
//      A C++ 11 allocator for containers to put
//      their elements in contiguous memory.
//      Since elements are packed and adjacent
//      to each other, the layout is cache optimised.
//      It allows a fetch of a cache line to contain
//      maximum number of container elements.
//----------------------------------------------------------------------------

#pragma once

#include <memory>
#include <cstddef>
#include <cstdlib>
#include <cassert>

namespace utils
{

/*
Notes:
1.  This allocator can be used for cache efficient
     access of container elements.
2.  It is meant to be used when the container
     has reached a state when no more insert
     or erase of elements are needed. For example
     a. 
     a dictionary that will be used for only lookup
     after it is initialized,
     b.
     a key-value store where only the value
     changes but not the key.
     Test file has usage examples.
*/

template<typename T>
class contiguous_allocator
{
public:
    using value_type = T;
    using pointer = T * ;
    using reference = T & ;
    using const_pointer = const T*;
    using const_reference = const T&;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    contiguous_allocator(size_t capacity) noexcept : capacity_{ capacity }, pmem_{ nullptr }, size_{ 0 }
    {
    }

    ~contiguous_allocator()
    {
        if (pmem_)
        {
            capacity_ = 0;
            size_ = 0;
            free(pmem_);
            pmem_ = nullptr;
        }
    }

    // must be defined to allow for copy-constructions from allocator objects of other types.
    template <class U>
    contiguous_allocator(const contiguous_allocator<U> & other) noexcept : capacity_{ other.capacity_ }, pmem_{ nullptr }, size_{ 0 }
    {
    }

    template <typename Type>
    struct rebind
    {
        using other = contiguous_allocator<Type>;
    };

    pointer allocate(size_type n, std::allocator<void>::const_pointer hint = 0)
    {
        if (!pmem_)
        {
            assert(size_ == 0);
            pmem_ = pointer(malloc(capacity_ * sizeof(T)));
        }

        if (size_ + n <= capacity_)
        {
            auto p = pmem_ + size_;
            size_ += n;
            return p;
        }
        else
        {
            throw std::bad_alloc{};
        }
    }

    void deallocate(pointer p, size_type n)
    {
    }
private:
    size_t capacity_;
    pointer pmem_;
    size_t size_;

    template <class U> friend class contiguous_allocator;
};

} // namespace utils
