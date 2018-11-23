----------------------------------------------------------------------
unordered_map constructed with 0 elements

Visual studio:
construct allocator for pair<K,V>
copy construct allocator for _List_node <pair<K,V>>
	allocate 1 _List_node
		construct 2 _List_node*
copy construct allocator for _List_unchecked_iterator<...>
	allocate 16 _List_unchecked_iterator<...>
		construct 16 of above
destruct allocator for pair<K,V>

gcc:
construct allocator for pair<K,V>
copy construct allocator for _detail_Hash_node <pair<K,V>>
copy construct allocator for _detail_Hash_node <pair<K,V>>
destruct allocator for _detail_Hash_node <pair<K,V>>
destruct allocator for pair<K,V>

clang:
construct allocator for pair<K,V>
copy construct allocator for _hash_value_type<V>
copy construct allocator for _hash_node <_hash_value_type<V>>*
copy construct allocator for _hash_node <_hash_value_type<V>>
destruct allocator for _hash_value_type<V>
destruct allocator for pair<K,V>

----------------------------------------------------------------------
unordered_map constructed with 0 elements
4 elements inserted

Visual studio:
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

gcc:
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

clang:
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

----------------------------------------------------------------------
