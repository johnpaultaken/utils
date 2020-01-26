//----------------------------------------------------------------------------
// year   : 2020
// author : John Paul
// email  : johnpaultaken@gmail.com
// source : https://github.com/johnpaultaken
// description :
//      A generic cache implementation in C++11.
//      When the cache gets full the least recently used is ejected.
//----------------------------------------------------------------------------

#pragma once

#include <unordered_map>
#include <list>
#include <string>
#include <sstream>

/*
Notes:
The cache service implements a cache which is organized as
1.
an unordered_map with cache lookup key as key
unordered_map<cache-lookup-key, {cached-value, iterator-to-item-in-2}>
note: iterators to list<> remain valid after insert or erase, except to erased
items.
2.
a list ordered by least recently used
list<{cache-lookup-key}>
note: cannot use iterator-to-item-in-1 because iterators to hash containers are
not valid after an insert.

Read Operation:-
When a key is looked up and found in 1 (O(1)),
cached-value is sent as response,
iterator-to-item-in-2 is used to locate item in 2 (O(1)),
item-in-2 is removed and moved to end (splice) (O(1)).

Write Operation:-
When a key is added to 1 (O(1)), first the cache size is checked (O(1)),
and if it is at capacity, the front of list is removed (O(1)), whose
cache-lookup-key is used to erase corresponding item from 1 (O(1)).
New key is added to 1 (O(1)), with corresponding item added to end of list (O(1)).
In case of existing key, the key's value is updated with no change to list
because typically preceding cache read for it would have updated it once.
Typically clients would have an expiry date-time inside cached-value,
and the client would update the cache when it finds cached-value has expired.
*/
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
        return true;
    }

    void put(const KEY & key, const VAL & val)
    {
        auto itr = _lookup.find(key);
        if (itr == _lookup.end())
        {
            if (_lookup.size() >= _capacity)
            {
                _lookup.erase(*_lru.begin());
                _lru.pop_front();
            }
            _lookup.emplace(key, value_type{val, _lru.insert(_lru.end(), key)});
        }
        else
        {
            // Just update _lookup. No change to _lru.
            itr->second._val = val;
        }
    }

    inline size_t size()
    {
        return _lookup.size();
    }

    inline size_t capacity()
    {
        return _capacity;
    }

    // check the consistency of internal data structures.
    // Params:
    //        details: OUT returns the detailed consistency check results.
    // Return: true if ok.
    bool check_consistency(std::string & details)
    {
        bool ret = true;
        std::ostringstream oss;
        for(const auto & key : _lru)
        {
            if (_lookup.find(key) == _lookup.end())
            {
                ret = false;
                oss << " " << key << ":error";
            }
            else
            {
                oss << " " << key << ":ok";
            }
        }
        details = oss.str();
        return ret;
    }
private:
    using list_type = list<KEY>;
    struct value_type{
        VAL _val;
        typename list_type::iterator _itr_lru;
    };
    using map_type = unordered_map<KEY, value_type>;

    size_t _capacity;
    map_type _lookup;
    list_type _lru;
};
}
