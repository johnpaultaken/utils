#include "contiguous_allocator.h"
#include "../test/test.h"

using namespace utils;

using std::cout;
#include <map>
using std::map;
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
    std::allocator_traits<contiguous_allocator<s>> t;
    ASSERT_M (sizeof(t), "allocator_traits compile time check");
}

size_t safesize_stdmap(size_t sz)
{
   return (
               sz
#ifdef _MSC_VER
               + 1     // Visual Studio needs one more for std::map.
#endif
   );
}

void test_stdmap()
{
    map <unsigned int, string> dict { 
        { 1,"one" }, { 2,"two" }, { 3,"three" }, { 4,"four" }
    };

    map<
        unsigned int, string,
        less<unsigned int>,
        contiguous_allocator <std::pair <const unsigned int, string>>
    > cache_optimized_dict(
        contiguous_allocator <map <unsigned int, string>::value_type> {
            safesize_stdmap(dict.size())
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
        contiguous_allocator <std::pair <const unsigned int, string>>
    > cache_optimized_dict(
        dict.begin(), dict.end(),
        std::less<unsigned int>(),
        contiguous_allocator <map <unsigned int, string>::value_type> {
            safesize_stdmap(dict.size())
        }
    );

    try
    {
        map<
            unsigned int, string,
            less<unsigned int>,
            contiguous_allocator <std::pair <const unsigned int, string>>
        > cache_optimized_dict_copy {
            cache_optimized_dict
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
    //getchar();
    return 0;
}
