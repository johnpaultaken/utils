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

#include<map>
#include<vector>

namespace utils
{
using std::map;
using std::vector;

template<typename KEY, typename VAL>
class cache
{
    cache(size_t capacity = 1024) : _capacity(capacity)
    {
    }
private:
    size_t _capacity;
    using value_type = struct{VAL val; size_t heap_index;};
    using map_type = map<KEY, value_type>;
    map_type _lookup;
    vector<struct{TIME timestamp; map_type::iterator_type itr_lookup;}> _heap;
};
}
