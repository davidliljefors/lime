#pragma once

#include <vector>

using u32 = unsigned int;
using u64 = unsigned long long;

template<typename T>
using Array = std::vector<T>;

namespace hashtable
{

template<typename T>
struct Hashtable
{
	struct Entry
	{
		u64 key;
		u32 next;
		T value;
	};

	// Entry defined in DDF like

	// KeyType Key (u64 hash or similar)
	// u32 Next;
	// EntryType Value; <- Value to store in the hash
	// Need to implement modulo for key signature u32 keymod(KeyType, u32 modulo);

	Array<u32> hash;
	Array<Entry> data;
};

constexpr u32 NULL_ENTRY = u32(-1);

struct HashFind
{
	u32 hashIndex;
	u32 dataPrev;
	u32 dataIndex;
};

template<typename Entry>
bool isFull(const Hashtable<Entry>& h);

template<typename Entry, typename Key>
u32 addEntry(Hashtable<Entry>& h, Key key);

template<typename Entry, typename Key, typename Value>
void insert(Hashtable<Entry>& h, Key key, Value&& value);

template<typename Entry, typename Key>
void erase(Hashtable<Entry>& h, Key key);

template<typename Entry>
void rehash(Hashtable<Entry>& h, u32 newSize);

template<typename Entry>
void grow(Hashtable<Entry>& h);


template<typename Entry, typename Key>
HashFind findImpl(Hashtable<Entry>& h, Key key)
{
	HashFind find;
	find.hashIndex = NULL_ENTRY;
	find.dataPrev = NULL_ENTRY;
	find.dataIndex = NULL_ENTRY;

	if (h.hash.empty())
		return find;

	find.hashIndex = key % h.hash.size();
	find.dataIndex = h.hash[find.hashIndex];
	while (find.dataIndex != NULL_ENTRY)
	{
		if (h.data[find.dataIndex].key == key)
		{
			return find;
		}
		find.dataPrev = find.dataIndex;
		find.dataIndex = h.data[find.dataIndex].next;
	}

	return find;
}

template<typename Entry>
bool isFull(const Hashtable<Entry>& h)
{
	constexpr float kMaxLoadFactor = 0.7f;
	return (float)h.data.size() >= (float)h.hash.size() * kMaxLoadFactor;
}

template<typename T, typename Key>
u32 addEntry(Hashtable<T>& h, Key key)
{
	typename Hashtable<T>::Entry e;
	e.key = key;
	e.next = NULL_ENTRY;
	u32 ei = h.data.size();
	h.data.push_back(e);
	return ei;
}

template<typename Entry>
void eraseImpl(Hashtable<Entry>& h, const HashFind& find)
{
	if (find.dataPrev == NULL_ENTRY)
		h.hash[find.hashIndex] = h.data[find.dataIndex].next;
	else
		h.data[find.dataPrev].next = h.data[find.dataIndex].next;

	if (find.dataIndex == h.data.size() - 1)
	{
		h.data.resize(h.data.size() - 1);
		return;
	}

	h.data[find.dataIndex] = h.data[h.data.size() - 1];
	HashFind last = findImpl(h, h.data[find.dataIndex].key);

	if (last.dataPrev != NULL_ENTRY)
	{
		h.data[last.dataPrev].setNext(find.dataIndex);
	}
	else
	{
		h.hash[last.hashIndex] = find.dataIndex;
	}
}

template<typename Entry, typename Key>
void erase(Hashtable<Entry>& h, Key key)
{
	const HashFind find = findImpl(h, key);
	if (find.dataIndex != NULL_ENTRY)
		eraseImpl(h, find);
}

template<typename Entry, typename Key>
u32 insertImpl(Hashtable<Entry>& h, Key key)
{
	const HashFind find = findImpl(h, key);
	const u32 i = addEntry(h, key);

	if (find.dataPrev == NULL_ENTRY)
	{
		h.hash[find.hashIndex] = i;
	}
	else
	{
		h.data[find.dataPrev].next = i;
	}

	h.data[i].next = find.dataIndex;
	return i;
}

template<typename Entry, typename Key, typename Value>
void insert(Hashtable<Entry>& h, Key key, Value&& value)
{
	if (h.hash.empty())
		hashtable::grow(h);

	const u32 i = hashtable::insertImpl(h, key);
	
	memcpy(&h.data[i].value, &value, sizeof(Value));
	memset(&value, 0, sizeof(Value));

	if (hashtable::isFull(h))
	{
		hashtable::grow(h);
	}
}

template<typename T>
void rehash(Hashtable<T>& h, u32 newSize)
{
	Hashtable<T> nh;


	nh.hash.resize(newSize);
	nh.data.reserve(h.data.size());

	for (u32 i = 0; i < newSize; ++i)
	{
		nh.hash[i] = NULL_ENTRY;
	}

	for (u32 i = 0; i < h.data.size(); ++i)
	{
		auto& e = h.data[i];
		hashtable::insert(nh, e.key, static_cast<T&&>(e.value));
	}

	if (h.hash.empty())
	{
		h.hash.swap(nh.hash);
		//h.hash.attach(nh.hash.detach());
	}
	else
	{
		h.data.swap(nh.data);
		h.hash.swap(nh.hash);
	}
}

template<typename Entry>
void grow(Hashtable<Entry>& h)
{
	const u32 newSize = h.data.size() * 2 + 10;
	rehash(h, newSize);
}

}