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
// Generic version.
// Template Params:
//      Container - the container type this allocator is going to be used for.
//                  Allocator allocation pattern is optimized for different
//                  container types. See readme.
//      T         - the usual allocation type param.
//      void      - this unnamed parameter is used to choose specialization
//                  based on traits of type T (using a technique called SFINAE)
//
template <template <class... Args> class Container, class T, class = void>
class contiguous_stdcontainer_allocator : public contiguous_allocator<T>
{
public:
    using base_type = contiguous_allocator<T>;

    //
    // Forward all base class constructors.
    // Params:
    //     capacity - same value as the intended capacity of the container
    //                this allocator is used for.
    //
    using base_type::base_type;

    //
    // Need to override rebind.
    //
    template <typename Type>
    struct rebind
    {
        using other = contiguous_stdcontainer_allocator<Container, Type>;
    };
};

//
// Specialized version for std::map.
//
template <class T>
class contiguous_stdcontainer_allocator<::std::map, T, void> :
    public contiguous_allocator<T>
{
public:
    using base_type = contiguous_allocator<T>;

    //
    // Params:
    //     capacity - same value as the intended capacity of the container
    //                this allocator is used for.
    //
    explicit contiguous_stdcontainer_allocator(size_t capacity) noexcept :
        base_type{
            capacity
    #ifdef _MSC_VER
            + 1     // Visual Studio needs one more for std::map.
    #endif
        }
    {
    }

    contiguous_stdcontainer_allocator(
        const contiguous_stdcontainer_allocator & other
    ) noexcept : base_type{other}
    {
    }

    //
    // Must be defined to allow for copy-constructions from allocator objects
    // of other types. See rebind below.
    //
    template <class U>
    contiguous_stdcontainer_allocator(
        const contiguous_stdcontainer_allocator<::std::map, U> & other
    ) noexcept : base_type{other}
    {
    }

    //
    // Need to override rebind.
    //
    template <typename Type>
    struct rebind
    {
        using other = contiguous_stdcontainer_allocator<::std::map, Type>;
    };
};

template<class... Args>
using void_t = void;

template<class T, class = void>
struct is_ptr_or_itr :
    public ::std::false_type
{
};

template<class T>
struct is_ptr_or_itr<T, void_t<decltype(*::std::declval<T>())>> :
    public ::std::true_type
{
};

//
// Specialized version for std::unordered_map hash table allocations.
// Used for allocating the hash table.
// Not used for allocating nodes wrapping the container elements itself.
// Note that in gcc and clang hash table is a table of pointers whereas
// in Visual Studio it is a table of iterators.
// Hash table is allocated as a block whereas nodes are allocated one by one.
// See readme for more details.
//
template <class T>
class contiguous_stdcontainer_allocator<
    ::std::unordered_map,
    T,
    typename ::std::enable_if< is_ptr_or_itr<T>::value, void>::type
> : public contiguous_allocator<T>
{
public:
    using base_type = contiguous_allocator<T>;
    using typename base_type::pointer;
    using typename base_type::size_type;

    //
    // Forward all base class constructors.
    // Params:
    //     capacity - same value as the intended capacity of the container
    //                this allocator is used for.
    //
    using base_type::base_type;

    //
    // Need to override rebind.
    //
    template <typename Type>
    struct rebind
    {
        using other =
            contiguous_stdcontainer_allocator<::std::unordered_map, Type>;
    };

    pointer allocate(
        size_type n, ::std::allocator<void>::const_pointer /*hint*/ = 0
    )
    {
        ctrace << "\n->allocate: " << n << " of type " << TYPENAME(T)
            << " of size " << sizeof(T);

        auto p = pointer(malloc(n * sizeof(T)));

        ctrace << "\n\tallocator: " << ::std::hex << this
            << " allocate & return address " << p << ::std::dec;

        return p;
    }

    void deallocate(pointer p, size_type n)
    {
        ctrace << "\n->deallocate: " << n << " of type " << TYPENAME(T)
            << " of size " << sizeof(T) << "\n\tat address " << ::std::hex << p
            << ::std::dec;

        free(p);

        ctrace << "\n\tallocator: " << ::std::hex << this
            << " freed address " << p << ::std::dec;
    }
};


//
// Specialized version for std::unordered_map node allocations.
// Used for allocating nodes wrapping the container elements itself.
// Not used for allocating the hash table.
// See readme for more details.
//
template <class T>
class contiguous_stdcontainer_allocator<
    ::std::unordered_map,
    T,
    typename ::std::enable_if< ! is_ptr_or_itr<T>::value, void>::type
> : public contiguous_allocator<T>
{
public:
    using base_type = contiguous_allocator<T>;

    //
    // Params:
    //     capacity - same value as the intended capacity of the container
    //                this allocator is used for.
    //
    explicit contiguous_stdcontainer_allocator(size_t capacity) noexcept :
        base_type{
            capacity
    #ifdef _MSC_VER
            + 1     // Visual Studio needs one more for unordered_map nodes.
    #endif
        }
    {
    }

    contiguous_stdcontainer_allocator(
        const contiguous_stdcontainer_allocator & other
    ) noexcept : base_type{other}
    {
    }

    //
    // Must be defined to allow for copy-constructions from allocator objects
    // of other types. See rebind below.
    //
    template <class U>
    contiguous_stdcontainer_allocator(
        const contiguous_stdcontainer_allocator<::std::unordered_map, U> & other
    ) noexcept : base_type{other}
    {
    }

    //
    // Need to override rebind.
    //
    template <typename Type>
    struct rebind
    {
        using other =
            contiguous_stdcontainer_allocator<::std::unordered_map, Type>;
    };
};

} // namespace utils
