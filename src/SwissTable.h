#pragma once

using u8 = unsigned char;
using u32 = unsigned int;
using u64 = unsigned long long;

constexpr u64 next_power_of_2(u64 n)
{
	if (n == 0) return 1;

	n |= n >> 1;
	n |= n >> 2;
	n |= n >> 4;
	n |= n >> 8;
	n |= n >> 16;
	n |= n >> 32;

	return n+1;
}

constexpr u64 power_of_2(u64 n)
{
	if (n == 0) return 1;

	--n;

	n |= n >> 1;
	n |= n >> 2;
	n |= n >> 4;
	n |= n >> 8;
	n |= n >> 16;
	n |= n >> 32;

	return n+1;
}

enum ControlByte : u8
{
	EMPTY = 0x80,
	DELETED = 0xFE,
};

inline void* st_alloc(u64 sz)
{
	return malloc(sz);
}

inline void st_free(void* mem)
{
	free(mem);
}

template<typename T>
class SwissTable
{
	struct Entry
	{
		u64 key;
		T value;
	};

	constexpr static float MAX_LOAD_FACTOR = 0.7f;

public:
	explicit SwissTable(u32 initialCapacity = 16);

	SwissTable(const SwissTable& r);
	SwissTable(SwissTable&& r);

	SwissTable& operator=(const SwissTable& r);
	SwissTable& operator=(SwissTable&& r);

	void insert(u64 key, const T& value);
	T* insert_uninit(u64 key);

	void insert_or_assign(u64 key, const T& value);

	T* find(u64 key);

	void erase(u64 key);

private:
	void init(u32 newCapacity);

	u32 hash(u64 key) const;

	u32 probe(u32 index, u32 step) const;

	u32 find_slot(u64 key) const;

	void rehash(u32 newCapacity);

public:
	u8* control;
	Entry* data;
	u32 size;
	u32 capacity;
};

template <typename T>
SwissTable<T>::SwissTable(u32 initialCapacity)
{
	init(power_of_2(initialCapacity));
}

template <typename T>
SwissTable<T>::SwissTable(const SwissTable& r)
{
	control = (u8*)st_alloc(r.capacity * sizeof(u8));
	data = (Entry*)st_alloc(r.capacity * sizeof(Entry));
	capacity = r.capacity;
	size = r.size;

	for (u32 i = 0; i < capacity; ++i)
	{
		control[i] = r.control[i];
		data[i] = r.data[i];
	}
}

template <typename T>
SwissTable<T>::SwissTable(SwissTable&& r)
{
	if (this != &r)
	{
		data = r.data;
		control = r.control;
		capacity = r.capacity;
		size = r.size;


		r.control = nullptr;
		r.data = nullptr;
		r.size = 0;
		r.capacity = 0;
	}
}

template <typename T>
SwissTable<T>& SwissTable<T>::operator=(const SwissTable& r)
{
	if (this != &r)
	{
		// todo free
		capacity = r.capacity;
		size = r.size;


		control = (u8*)st_alloc(capacity * sizeof(u8));
		memset(control, EMPTY, capacity);
		data = (Entry*)st_alloc(capacity * sizeof(Entry));
		memset(data, 0, capacity * sizeof(Entry));


		for (u32 i = 0; i < capacity; ++i)
		{
			if (r.control[i] != DELETED && r.control[i] != EMPTY)
			{
				control[i] = r.control[i];
				data[i] = r.data[i];
			}
		}
	}

	return *this;
}

template <typename T>
SwissTable<T>& SwissTable<T>::operator=(SwissTable&& r)
{
	if (this != &r)
	{

		data = r.data;
		control = r.control;
		capacity = r.capacity;
		size = r.size;

		r.control = nullptr;
		r.data = nullptr;
		r.size = 0;
		r.capacity = 0;
	}

	return *this;
}

template <typename T>
void SwissTable<T>::insert(u64 key, const T& value)
{
	if (size >= capacity * MAX_LOAD_FACTOR)
		rehash(next_power_of_2(capacity));

	u32 index = find_slot(key);

	if (control[index] == EMPTY || control[index] == DELETED)
	{
		control[index] = key & 0x7F;
		data[index] = { key, value };
		size++;
	}
}

template <typename T>
T* SwissTable<T>::insert_uninit(u64 key)
{
	if (size >= capacity * MAX_LOAD_FACTOR)
		rehash(next_power_of_2(capacity));

	u32 index = find_slot(key);

	if (control[index] == EMPTY || control[index] == DELETED)
	{
		control[index] = key & 0x7F;
		data[index].key = key;
		size++;
		return &data[index].value;
	}

	return nullptr;
}

template <typename T>
void SwissTable<T>::insert_or_assign(u64 key, const T& value)
{
	if (size >= capacity * 0.7)
		rehash(next_power_of_2(capacity));

	u32 index = find_slot(key);

	if (control[index] == EMPTY || control[index] == DELETED)
	{
		control[index] = key & 0x7F;
		data[index] = { key, value };
		size++;
	}
	else
	{
		data[index] = { key, value };
	}
}

template <typename T>
T* SwissTable<T>::find(u64 key)
{
	u32 index = find_slot(key);
	
	if (control[index] != EMPTY && data[index].key == key)
	{
		return &data[index].value;
	}

	return nullptr;
}

template <typename T>
void SwissTable<T>::erase(u64 key)
{
	u32 index = find_slot(key);
	if (control[index] != EMPTY && data[index].key == key)
	{
		control[index] = DELETED;
		size--;
	}
}

template <typename T>
void SwissTable<T>::init(u32 newCapacity)
{
	size = 0;
	capacity = newCapacity;
	control = (u8*)st_alloc(capacity * sizeof(u8));
	memset(control, EMPTY, capacity);
	data = (Entry*)st_alloc(capacity * sizeof(Entry));
	memset(data, 0, capacity * sizeof(Entry));

}

template <typename T>
u32 SwissTable<T>::hash(u64 key) const
{
	return key & (capacity - 1);
}

template <typename T>
u32 SwissTable<T>::probe(u32 index, u32 step) const
{
	return (index + step * step) & (capacity - 1);
}

template <typename T>
u32 SwissTable<T>::find_slot(u64 key) const
{
	u32 index = hash(key);
	u32 step = 1;

	while (control[index] != EMPTY)
	{
		if (control[index] != DELETED && data[index].key == key)
			return index;

		index = probe(index, step++);
	}
	return index;
}

template <typename T>
void SwissTable<T>::rehash(u32 newCapacity)
{
	u8* oldControl = control;
	Entry* oldData = data;
	u32 oldCapacity = capacity;

	capacity = newCapacity;

	control = (u8*)st_alloc(capacity * sizeof(u8));
	memset(control, EMPTY, capacity);
	data = (Entry*)st_alloc(capacity * sizeof(Entry));
	memset(data, 0, capacity * sizeof(Entry));

	for (u32 i = 0; i < oldCapacity; i++)
	{
		if (oldControl[i] != EMPTY && oldControl[i] != DELETED)
		{
			T* dst = insert_uninit(oldData[i].key);
			memcpy(dst, &oldData[i].value, sizeof(T));
		}
	}

	st_free(oldControl);
	st_free(oldData);
}


