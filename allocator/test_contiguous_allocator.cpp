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
}

int main()
{
    test_compiletime_check();
    test_stdmap();

    std::cout << "\n done";
    getchar();
    return 0;
}