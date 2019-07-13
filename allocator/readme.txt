==============================================================================
std::unordered_map implementation uses a hash table.
This requires two allocator types, one for the hash table and another for the
map element.

The hash table is allocated as a chunk of pointers to element.
This chunk is reallocated to increase the table size as the container size
increases.
On growth, a larger chunk is allocated, the smaller chunk is copied over, and
the smaller chunk is released.
Note the reallocation size sequences for unordered_map(itrBegin, itrEnd)
clang:          2   5   11  23  47  97  197 397 797 1597 ...
visual studio:  16      128     1024    2048    4096 ...  
gcc:            No reallocation if size is known in advance. gcc is smarter.

However, map elements are allocated one by one as the container size increases.
Hence there is no reallocate involved in this case.

So the allocator capacity needed to support unordered_map is different
for pointer type vs non-pointer type.
contiguous_stdcontainer_allocator is provided to support such special needs.
==============================================================================
Challenge:
Can you figure out the next number in the above clang sequence ?
Hint:
Statistical analysis have shown hash table sizing to keep certain properties
to enable even distribution.
==============================================================================
unordered_map constructed with 1033 elements
unordered_map(itrBegin, itrEnd)

+========+
|  gcc   |.................................................
+========+
note: good allocation pattern of ptr to node

........................
ptr to node allocations:-

1109 of ptr to node allocated as one block

........................
node allocations:-

1 of node allocated 1033 times

+=========+
|  clang  |...............................................
+=========+
note: good allocation pattern of ptr to node

........................
ptr to node allocations:-

1033 of ptr to node allocated as one block

........................
node allocations:-

1 of node allocated 1033 times

+=================+
|  visual studio  |........................................
+=================+
note: bad allocation pattern of ptr to node.
Allocates too much wasted space. pow(2, ceil(log2(size))) * 2 * 2
One unnecessary realloc.

........................
ptr to node allocations:-

16 block allocate

4096 block allocate
copy old block to new block
16 block deallocate

........................
node allocations:-

1 of node allocated 1033 times

==============================================================================
unordered_map constructed with allocator unordered_map(allocator)
1033 elements inserted using insert(itrBegin, itrEnd)

+========+
|  gcc   |.................................................
+========+
note: good allocation pattern of ptr to node
No reallocs and copy.

........................
ptr to node allocations:-

1109 of ptr to node allocated as one block

........................
node allocations:-

1 of node allocated 1033 times

+=========+
|  clang  |...............................................
+=========+
note: bad allocation pattern of ptr to node
Too many reallocs and copy.

........................
ptr to node allocations:-

2 block allocate

5 block allocate
copy to new block
2 block deallocate

11 block allocate
copy to new block
5 block deallocate

23 block allocate
copy to new block
11 block deallocate

(... continue growing and copying until..)

1597 block allocate
copy to new block
797 block deallocate

........................
node allocations:-

1 of node allocated 1033 times

+=================+
|  visual studio  |........................................
+=================+
note: bad allocation pattern of ptr to node
Allocates too much wasted space. pow(2, ceil(log2(size))) * 2 * 2
Also involves reallocs and copy.

........................
ptr to node allocations:-

16 block allocate

128 block allocate
copy to new block
16 block deallocate

1024 block allocate
copy to new block
128 block deallocate

2048 block allocate
copy to new block
1024 block deallocate

4096 block allocate
copy to new block
2048 block deallocate

........................
node allocations:-

1 of node allocated 1033 times

==============================================================================
unordered_map constructed with 0 elements

+========+
|  gcc   |
+========+
construct allocator for pair<K,V>
copy construct allocator for _detail_Hash_node <pair<K,V>>
copy construct allocator for _detail_Hash_node <pair<K,V>>
destruct allocator for _detail_Hash_node <pair<K,V>>
destruct allocator for pair<K,V>

+=========+
|  clang  |
+=========+
construct allocator for pair<K,V>
copy construct allocator for _hash_value_type<V>
copy construct allocator for _hash_node <_hash_value_type<V>>*
copy construct allocator for _hash_node <_hash_value_type<V>>
destruct allocator for _hash_value_type<V>
destruct allocator for pair<K,V>

+=================+
|  visual studio  |
+=================+
construct allocator for pair<K,V>
copy construct allocator for _List_node <pair<K,V>>
    allocate 1 _List_node
        construct 2 _List_node*
copy construct allocator for _List_unchecked_iterator<...>
    allocate 16 _List_unchecked_iterator<...>
        construct 16 of above
destruct allocator for pair<K,V>

==============================================================================
unordered_map constructed with 0 elements
4 elements inserted

+========+
|  gcc   |
+========+
construct allocator for pair<K,V>
copy construct allocator for _detail_Hash_node <pair<K,V>>
destruct allocator for pair<K,V>
copy construct allocator for _detail_Hash_node *
	allocate 5 of above type
destruct allocator for _detail_Hash_node_base
	allocate: 1 of _detail_Hash_node <pair<K,V>>
		construct 1 of pair<K,V>
	allocate: 1 of _detail_Hash_node <pair<K,V>>
		construct 1 of pair<K,V>
	allocate: 1 of _detail_Hash_node <pair<K,V>>
		construct 1 of pair<K,V>
	allocate: 1 of _detail_Hash_node <pair<K,V>>
		construct 1 of pair<K,V>

+=========+
|  clang  |
+=========+
construct allocator for pair<K,V>
copy construct allocator for _hash_value_type<V>
copy construct allocator for _hash_node <_hash_value_type<V>> *
copy construct allocator for _hash_node <_hash_value_type<V>>
destruct allocator for _hash_value_type<V>
destruct allocator for pair<K,V>
	allocate 1 _hash_node <_hash_value_type<V>>
	allocate 2 _hash_node <_hash_value_type<V>> *
	allocate 1 _hash_node <_hash_value_type<V>>
	allocate 1 _hash_node <_hash_value_type<V>>
	allocate 5 _hash_node <_hash_value_type<V>> *
	deallocate: 2 of _hash_node <_hash_value_type<V>> *
	allocate 1 _hash_node <_hash_value_type<V>>

+=================+
|  visual studio  |
+=================+
construct allocator for pair<K,V>
copy construct allocator for _List_node <pair<K,V>>
    allocate 1 _List_node
        construct 2 _List_node*
copy construct allocator for _List_unchecked_iterator<...>
    allocate 16 _List_unchecked_iterator<...>
        construct 16 of above
destruct allocator for pair<K,V>
    allocate 1 of _List_node
        construct 2 _List_node*
        construct pair<K,V>
    allocate 1 of _List_node
        construct 2 _List_node*
        construct pair<K,V>
    allocate 1 of _List_node
        construct 2 _List_node*
        construct pair<K,V>
    allocate 1 of _List_node
        construct 2 _List_node*
        construct pair<K,V>

==============================================================================
