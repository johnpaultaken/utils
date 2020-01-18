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

#include <map>
#include <vector>
#include <chrono>

namespace utils
{
using std::map;
using std::vector;
using std::chrono::system_clock;

template<typename KEY, typename VAL>
class cache
{
public:
    cache(size_t capacity = 1024) : _capacity(capacity)
    {
    }

    // if found in cache, copies to val and return true.
    bool find(const KEY & key, VAL & val)
    {
        auto itr = _lookup.find(key);
        if (itr == _lookup.end())
        {
            return false;
        }
        else
        {
        }
    }
private:
    using value_type = struct{
        VAL val;
        size_t heap_index;
    };
    using map_type = map<KEY, value_type>;
    using heap_type = struct{
        system_clock::rep timestamp;
        typename map_type::iterator itr_lookup;
    };

    size_t _capacity;
    map_type _lookup;
    vector<heap_type> _heap;
};
}
