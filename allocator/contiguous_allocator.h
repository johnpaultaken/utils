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
//----------------------------------------------------------------------------

#pragma once

#include <memory>
#include <cstddef>
#include <cstdlib>
#include <cassert>

#include "../misc/utils.h"

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

    //
    // Param: capacity - usually same value as the intended capacity of the
    //          container this allocator is used for. However this is not
    //           enough for some containers like unordered_map. See readme.
    //
    explicit contiguous_allocator(size_t capacity) noexcept :
        capacity_{capacity}, pmem_{nullptr}, size_{0}, size_dealloc_{0}
    {
        ctrace << "\n->constructor: " << ::std::hex << this << ::std::dec
            << " allocator for type " << TYPENAME(T);
    }

    contiguous_allocator(const contiguous_allocator & other) noexcept :
        capacity_{other.capacity_}, pmem_{nullptr}, size_{0}, size_dealloc_{0}
    {
        ctrace << "\n->copy constructor: " << ::std::hex << this << ::std::dec
            << " allocator for type " << TYPENAME(T) << "\n\tfrom "
            << ::std::hex << &other << ::std::dec << " of same type.";

        if (other.pmem_)
        {
            ctrace << "\n\tcopy construction from a used contiguous_allocator.";
        }
    }

    //
    // Must be defined to allow for copy-constructions from allocator objects
    // of other types. See rebind below.
    //
    template <class U>
    contiguous_allocator(const contiguous_allocator<U> & other) noexcept :
        capacity_{other.capacity_}, pmem_{nullptr}, size_{0}, size_dealloc_{0}
    {
        ctrace << "\n->copy constructor: " << ::std::hex << this << ::std::dec
            << " allocator for type " << TYPENAME(T) << "\n\tfrom "
            << ::std::hex << &other << ::std::dec << " of type " << " "
            << TYPENAME(U);

        if (other.pmem_)
        {
            ctrace << "\n\tcopy construction from a used contiguous_allocator.";
        }
    }

    ~contiguous_allocator()
    {
        ctrace << "\n->destructor: " << ::std::hex << this << ::std::dec
            << " allocator for type " << TYPENAME(T);

        // This check is needed only due to Visual Studio bug.
        // Visual studio 2015 & 2017 std::map implementation continues to use
        // memory allocated by an allocator even after the allocator is freed.
        if (size_dealloc_ == size_)
        {
            if (pmem_)
            {
                capacity_ = 0;
                size_ = 0;
                free(pmem_);
                pmem_ = nullptr;
                size_dealloc_ = 0;
            }
        }
        else
        {
            ctrace << "\n\tWARNING: memory leak. Allocator destroyed before"
                << " all memory deallocated.";
        }
    }

    //
    // Containers that need allocation of type other than its template
    // parameter uses this to figure out type of allocator it needs.
    //
    template <typename Type>
    struct rebind
    {
        using other = contiguous_allocator<Type>;
    };

    pointer allocate(
        size_type n, ::std::allocator<void>::const_pointer /*hint*/ = 0
    )
    {
        ctrace << "\n->allocate: " << n << " of type " << TYPENAME(T)
            << " of size " << sizeof(T);

        if (!pmem_)
        {
            ctrace << "\n\tallocator: " << ::std::hex << this << ::std::dec
                << " init for capacity " << capacity_;

            assert(size_ == 0);
            pmem_ = pointer(malloc(capacity_ * sizeof(T)));
        }

        if (size_ + n <= capacity_)
        {
            auto p = pmem_ + size_;
            size_ += n;

            ctrace << "\n\tallocator: " << ::std::hex << this
                << " return address " << p << ::std::dec;

            return p;
        }
        else
        {
            ctrace << "\n\tFAIL allocator: " << ::std::hex << this
                << ::std::dec << " exceeded capacity." << ::std::flush;

            throw ::std::bad_alloc{};
        }
    }

    void deallocate(pointer p, size_type n)
    {
        ctrace << "\n->deallocate: " << n << " of type " << TYPENAME(T)
            << " of size " << sizeof(T) << "\n\tat address " << ::std::hex << p
            << ::std::dec;

        size_dealloc_ += n;

        // suppress unused parameter warning when TRACE is not defined.
        (void)(p);
    }

    //
    // C++11 allocator of one type can be asked to construct object of
    // another type.
    // For example, the constructed type can be embedded inside the allocated
    // type and might need independent in-place construction. Say, the
    // implementation of a container like map could have the value_type
    // embedded inside a Red-Black Tree node.
    //
    template<class U, class... Args>
    void construct (U* p, Args&&... args)
    {
        ctrace << "\n->construct: of type " << TYPENAME(U)
            << "\n\tby allocator for type " << TYPENAME(T)
            << "\n\tat address " << ::std::hex << p << ::std::dec;

        ::new ((void *)p) U(::std::forward<Args>(args)...);
    }

    template<class U>
    void destroy(U* p)
    {
        ctrace << "\n->destroy: of type " << TYPENAME(U)
            << "\n\tby allocator for type " << TYPENAME(T)
            << "\n\tat address " << ::std::hex << p << ::std::dec;

        p->~U();
    }

private:
    size_t capacity_;
    pointer pmem_;
    size_t size_;

    template <class U> friend class contiguous_allocator;

    // the following members are for sanity check only.
    size_t size_dealloc_;
};

} // namespace utils
