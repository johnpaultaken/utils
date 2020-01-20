//----------------------------------------------------------------------------
// year   : 2020
// author : John Paul
// email  : johnpaultaken@gmail.com
// source : https://github.com/johnpaultaken
// description :
//      A generic cache implementation in C++11.
//      When the cache gets full the least recently used is deleted.
//----------------------------------------------------------------------------

#pragma once

#include <unordered_map>
#include <list>

namespace utils
{
using std::unordered_map;
using std::list;

template<typename KEY, typename VAL>
class cache
{
public:
    cache(size_t capacity = 1024) : _capacity(capacity)
    {
    }

    // if found in cache, copies to val and return true.
    bool get(const KEY & key, VAL & val)
    {
        auto itr = _lookup.find(key);
        if (itr == _lookup.end())
        {
            return false;
        }
        else
        {
            val = itr->second._val;
            auto end_splice = itr->second._itr_lru; ++end_splice;
            _lru.splice( _lru.end(), _lru, itr->second._itr_lru, end_splice);
        }
    }
private:
    using list_type = list<const KEY>;
    using value_type = struct{
        VAL _val;
        typename list_type::iterator _itr_lru;
    };
    using map_type = unordered_map<KEY, value_type>;

    size_t _capacity;
    map_type _lookup;
    list_type _lru;
};
}

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

int main()
{
    utils::cache<string, page_cache_value> page_cache(4);
    page_cache_value cached_page;
    if (page_cache.get("http://rextester.com", cached_page))
    {
        cout << "timestamp:" << cached_page._timestamp << "\tpage: " << cached_page._page;
    }
    std::cout << "\ndone";
}
