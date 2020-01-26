#include "cache.h"
#include "../test/test.h"
using namespace utils;
#include <iostream>
using std::cout;
#include <string>
using std::string;
#include <chrono>
using std::chrono::system_clock;

struct page_cache_value
{
    string _page;
    system_clock::rep _timestamp;
};

void test_put()
{
    utils::cache<string, page_cache_value> page_cache(2);
    page_cache.put("http://abc.com", page_cache_value{ "yak yak", system_clock::now().time_since_epoch().count() });
    string ignore;
    ASSERT_M((page_cache.size() == 1) && page_cache.check_consistency(ignore), "cache put interface");
}

void test_get()
{
    utils::cache<string, page_cache_value> page_cache(2);
    page_cache.put("http://abc.com", page_cache_value{ "yak yak", system_clock::now().time_since_epoch().count() });
    page_cache_value cached_page;
    auto isok = page_cache.get("http://abc.com", cached_page);
    ASSERT_M(isok && cached_page._page == "yak yak", "cache put interface");
}

void test_interface_basic()
{
    test_put();
    test_get();
}

void test_put_update()
{
    utils::cache<string, page_cache_value> page_cache(2);
    page_cache.put("http://abc.com", page_cache_value{ "expired data", system_clock::now().time_since_epoch().count() });
    page_cache.put("http://abc.com", page_cache_value{ "yak yak", system_clock::now().time_since_epoch().count() });
    page_cache_value cached_page;
    auto isok = page_cache.get("http://abc.com", cached_page);
    ASSERT_M(page_cache.size()==1 && cached_page._page == "yak yak", "cache put update data");
}

void test_put_idempotent()
{
    utils::cache<string, page_cache_value> page_cache(2);
    page_cache.put("http://rextester.com", page_cache_value{ "blah blah", system_clock::now().time_since_epoch().count() });
    page_cache.put("http://abc.com", page_cache_value{ "yak yak", system_clock::now().time_since_epoch().count() });
    page_cache.put("http://abc.com", page_cache_value{ "yak yak", system_clock::now().time_since_epoch().count() });
    page_cache_value cached_page_abc;
    page_cache.get("http://abc.com", cached_page_abc);
    page_cache_value cached_page_rextester;
    page_cache.get("http://rextester.com", cached_page_rextester);
    ASSERT_M(cached_page_rextester._page == "blah blah" && cached_page_abc._page == "yak yak", "cache put is idempotent");
}

void test_capacity_eject_oldest()
{
    utils::cache<string, page_cache_value> page_cache(2);
    page_cache.put("http://abc.com", page_cache_value{ "yak yak", system_clock::now().time_since_epoch().count() });
    page_cache.put("http://rextester.com", page_cache_value{ "blah blah", system_clock::now().time_since_epoch().count() });
    page_cache.put("http://fakenews.com", page_cache_value{ "yada yada", system_clock::now().time_since_epoch().count() });

    page_cache_value cached_page;
    ASSERT_M(!page_cache.get("http://abc.com", cached_page), "oldest cache item ejection");
}

void test_capacity_eject_lru()
{
    page_cache_value cached_page;
    utils::cache<string, page_cache_value> page_cache(2);
    page_cache.put("http://abc.com", page_cache_value{ "yak yak", system_clock::now().time_since_epoch().count() });
    page_cache.put("http://rextester.com", page_cache_value{ "blah blah", system_clock::now().time_since_epoch().count() });
    page_cache.get("http://abc.com", cached_page);
    page_cache.put("http://fakenews.com", page_cache_value{ "yada yada", system_clock::now().time_since_epoch().count() });

    ASSERT_M(
        page_cache.get("http://abc.com", cached_page) && ! page_cache.get("http://rextester.com", cached_page),
        "least recently used cache item ejection"
    );
}

int main()
{
    test_interface_basic();
    test_put_update();
    test_put_idempotent();
    test_capacity_eject_oldest();
    test_capacity_eject_lru();

    std::cout << "\n done";
    //getchar();
    return 0;
}
