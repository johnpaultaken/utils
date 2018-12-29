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
using std::equal_to;
using std::hash;

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

void test_stdmap_insert()
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

void test_stdmap_moveconstruct()
{
    map <unsigned int, string> expected{
        { 1,"one" },{ 2,"two" },{ 3,"three" },{ 4,"four" }
    };

    std::shared_ptr<
        map<
            unsigned int, string,
            less<unsigned int>,
            contiguous_stdcontainer_allocator <
                map, std::pair <const unsigned int, string>
            >
        >
    > p_dict_move_to;

    try
    {
        {
            map<
                unsigned int, string,
                less<unsigned int>,
                contiguous_stdcontainer_allocator <
                    map, std::pair <const unsigned int, string>
                >
            > cache_optimized_dict(
                expected.begin(), expected.end(),
                std::less<unsigned int>(),
                contiguous_stdcontainer_allocator <
                    map, map <unsigned int, string>::value_type
                > {
                    expected.size()
                }
            );

            p_dict_move_to =
                std::make_shared<
                    map<
                        unsigned int, string,
                        less<unsigned int>,
                        contiguous_stdcontainer_allocator <
                            map, std::pair <const unsigned int, string>
                        >
                    >
                >(std::move(cache_optimized_dict))
            ;

            ASSERT_M(
                cache_optimized_dict.size() == 0,
                "move construct map using contiguous_allocator"
            );
        }

        map <unsigned int, string> actual{
            p_dict_move_to->begin(),
            p_dict_move_to->end()
        };

        ASSERT_M(
            actual == expected,
            "move construct map using contiguous_allocator"
        );
    }
    catch (...)
    {
        FAIL_M("move construct map using contiguous_allocator");
        return;
    }
}

void test_unordmap_insert()
{
    map <unsigned int, string> dict {
        { 1,"one" }, { 2,"two" }, { 3,"three" }, { 4,"four" }
    };

    unordered_map<
        unsigned int, string,
        hash<unsigned int>,
        equal_to<unsigned int>,
        contiguous_stdcontainer_allocator <
            unordered_map, std::pair <const unsigned int, string>
        >
    > cache_optimized_dict(
        contiguous_stdcontainer_allocator <
            unordered_map, unordered_map <unsigned int, string>::value_type
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
        FAIL_M ("insert into unordered_map using contiguous_allocator");
        return;
    }

    map <unsigned int, string> actual{
        cache_optimized_dict.begin(),
        cache_optimized_dict.end()
    };
    ASSERT_M (
        actual == dict, "insert into unordered_map using contiguous_allocator"
    );
}

void test_unordmap_copyconstruct()
{
    map <unsigned int, string> dict {
        { 1,"one" }, { 2,"two" }, { 3,"three" }, { 4,"four" }
    };

    unordered_map<
        unsigned int, string,
        hash<unsigned int>,
        equal_to<unsigned int>,
        contiguous_stdcontainer_allocator <
            unordered_map, std::pair <const unsigned int, string>
        >
    > cache_optimized_dict(
        dict.begin(), dict.end(),
        0,  // use implementation defined bucket_count
        hash<unsigned int>(),
        equal_to<unsigned int>(),
        contiguous_stdcontainer_allocator <
            unordered_map, unordered_map <unsigned int, string>::value_type
        > {
            dict.size()
        }
    );

    try
    {
        unordered_map<
            unsigned int, string,
            hash<unsigned int>,
            equal_to<unsigned int>,
            contiguous_stdcontainer_allocator <
                unordered_map, std::pair <const unsigned int, string>
            >
        > cache_optimized_dict_copy {
            cache_optimized_dict
        };

        map <unsigned int, string> actual{
            cache_optimized_dict_copy.begin(),
            cache_optimized_dict_copy.end()
        };

        ASSERT_M (
            actual == dict,
            "copy construct unordered_map using contiguous_allocator"
        );
    }
    catch (...)
    {
        FAIL_M ("copy construct unordered_map using contiguous_allocator");
        return;
    }
}

void test_unordmap_moveconstruct()
{
    map <unsigned int, string> expected{
        { 1,"one" },{ 2,"two" },{ 3,"three" },{ 4,"four" }
    };

    std::shared_ptr<
        unordered_map<
            unsigned int, string,
            hash<unsigned int>,
            equal_to<unsigned int>,
            contiguous_stdcontainer_allocator <
                unordered_map, std::pair <const unsigned int, string>
            >
        >
    > p_dict_move_to;

    try
    {
        {
            unordered_map<
                unsigned int, string,
                hash<unsigned int>,
                equal_to<unsigned int>,
                contiguous_stdcontainer_allocator <
                    unordered_map, std::pair <const unsigned int, string>
                >
            > cache_optimized_dict(
                expected.begin(), expected.end(),
                0,  // use implementation defined bucket_count
                hash<unsigned int>(),
                equal_to<unsigned int>(),
                contiguous_stdcontainer_allocator <
                    unordered_map, unordered_map <unsigned int, string>::value_type
                > {
                    expected.size()
                }
            );

            ctrace << "\n************************** before" << std::flush;

            p_dict_move_to =
                std::make_shared<
                    unordered_map<
                        unsigned int, string,
                        hash<unsigned int>,
                        equal_to<unsigned int>,
                        contiguous_stdcontainer_allocator <
                            unordered_map, std::pair <const unsigned int, string>
                        >
                    >
                >(std::move(cache_optimized_dict))
            ;

            ctrace << "\n************************** after " << cache_optimized_dict.size() << std::flush;

            ASSERT_M(
                cache_optimized_dict.size() == 0,
                "move construct unordered_map using contiguous_allocator"
            );
        }

        ctrace << "\n************************** destroyed" << std::flush;

        map <unsigned int, string> actual{
            p_dict_move_to->begin(),
            p_dict_move_to->end()
        };

        ASSERT_M(
            actual == expected,
            "move construct unordered_map using contiguous_allocator"
        );
    }
    catch (...)
    {
        FAIL_M("move construct unordered_map using contiguous_allocator");
        return;
    }
}

int main()
{
    test_compiletime_check();

    test_stdmap_insert();
    test_stdmap_copyconstruct();
    test_stdmap_moveconstruct();

    test_unordmap_insert();
    test_unordmap_copyconstruct();
    test_unordmap_moveconstruct();

    std::cout << "\n done";
    //getchar();
    return 0;
}
