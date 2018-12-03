#include "contiguous_stdcontainer_allocator.h"
#include "../test/test.h"

using namespace utils;

using std::cout;
#include <map>
using std::map;
#include <unordered_map>
using std::unordered_map;
#include <list>
using std::list;
#include <string>
using std::string;
using std::less;

struct s
{
    double d_;
    float f_;
    s(int i) : d_(double(i)), f_(float(i))
    {
    }
    bool operator < (const s & rhs) const
    {
        return d_ < rhs.d_;
    }
};

void test_compiletime_check()
{
    std::allocator_traits<contiguous_stdcontainer_allocator<list, s>> a1;
    ASSERT_M (sizeof(a1), "allocator_traits compile time check");

    std::allocator_traits<contiguous_stdcontainer_allocator<map, s>> a2;
    ASSERT_M (sizeof(a2), "allocator_traits compile time check");

    std::allocator_traits<
        contiguous_stdcontainer_allocator<unordered_map, s*>
    > a3;
    ASSERT_M (sizeof(a3), "allocator_traits compile time check");
}

void test_stdmap()
{
    map <unsigned int, string> dict { 
        { 1,"one" }, { 2,"two" }, { 3,"three" }, { 4,"four" }
    };

    map<
        unsigned int, string,
        less<unsigned int>,
        contiguous_stdcontainer_allocator <
            map, std::pair <const unsigned int, string>
        >
    > cache_optimized_dict(
        contiguous_stdcontainer_allocator <
            map, map <unsigned int, string>::value_type
        > {
            dict.size()
        }
    );

    try
    {
        cache_optimized_dict.insert (dict.begin(), dict.end());
    }
    catch (...)
    {
        FAIL_M ("insert into map using contiguous_allocator");
        return;
    }

    map <unsigned int, string> actual{
        cache_optimized_dict.begin(),
        cache_optimized_dict.end()
    };
    ASSERT_M (actual == dict, "insert into map using contiguous_allocator");
}

void test_stdmap_copyconstruct()
{
    map <unsigned int, string> dict {
        { 1,"one" }, { 2,"two" }, { 3,"three" }, { 4,"four" }
    };

    map<
        unsigned int, string,
        less<unsigned int>,
        contiguous_stdcontainer_allocator <
            map, std::pair <const unsigned int, string>
        >
    > cache_optimized_dict(
        dict.begin(), dict.end(),
        std::less<unsigned int>(),
        contiguous_stdcontainer_allocator <
            map, map <unsigned int, string>::value_type
        > {
            dict.size()
        }
    );

    try
    {
        map<
            unsigned int, string,
            less<unsigned int>,
            contiguous_stdcontainer_allocator <
                map, std::pair <const unsigned int, string>
            >
        > cache_optimized_dict_copy {
            cache_optimized_dict,
            cache_optimized_dict.size()
        };

        map <unsigned int, string> actual{
            cache_optimized_dict_copy.begin(),
            cache_optimized_dict_copy.end()
        };

        ASSERT_M (
            actual == dict,
            "copy construct map using contiguous_allocator"
        );
    }
    catch (...)
    {
        FAIL_M ("copy construct map using contiguous_allocator");
        return;
    }
}

int main()
{
    test_compiletime_check();
    test_stdmap();
    test_stdmap_copyconstruct();

    std::cout << "\n done";
    getchar();
    return 0;
}
