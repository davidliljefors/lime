#pragma once

#include <cstdlib>
#include <cstring>

#define interface struct

using i32 = int;
using u64 = unsigned long long;

interface Allocator
{
	virtual ~Allocator() = default;

	virtual void* alloc(i32 size) = 0;
	virtual void free(void* block) = 0;
};

class MallocAllocator : public Allocator
{
public:
	MallocAllocator() = default;
	~MallocAllocator() override = default;

	void* alloc(i32 size) override
	{
		return ::malloc(size);
	}

	void free(void* block) override
	{
		return ::free(block);
	}
};


template<typename T>
class Array
{
public:
	explicit Array(Allocator& allocator);
	~Array();

	void push_back(const T& val);

	void resize(i32 new_size);
	void reserve(i32 new_capacity);

	T& operator[](i32 i);
	const T& operator[](i32 i) const;


	T* begin();
	T* end();

private:
	void destroy(i32 i);
	void grow();
private:
	Allocator& m_allocator;

	T* m_data = nullptr;
	i32 m_size = 0;
	i32 m_capacity = 0;
};

template <typename T>
Array<T>::Array(Allocator& allocator)
	: m_allocator(allocator)
{

}

template <typename T>
Array<T>::~Array()
{
	for (i32 i = 0; i < m_size; ++i)
	{
		destroy(i);
	}

	if (m_data)
	{
		m_allocator.free(m_data);
	}

	m_capacity = 0;
	m_size = 0;
}

template <typename T>
void Array<T>::push_back(const T& val)
{
	if (m_size == m_capacity)
		grow();

	m_data[m_size++] = val;
}

template <typename T>
void Array<T>::resize(i32 new_size)
{
}

template <typename T>
void Array<T>::reserve(i32 new_capacity)
{
}

template <typename T>
T& Array<T>::operator[](i32 i)
{
	return m_data[i];
}

template <typename T>
const T& Array<T>::operator[](i32 i) const
{
	return m_data[i];
}

template <typename T>
T* Array<T>::begin()
{
	return m_data;
}

template <typename T>
T* Array<T>::end()
{
	return m_data + m_size;
}

template <typename T>
void Array<T>::destroy(i32 i)
{
	m_data[i].~T();
}

template <typename T>
void Array<T>::grow()
{
	i32 new_capacity = m_capacity <= 8 ? 16 : (m_capacity * 2);
	
	T* new_data = (T*)m_allocator.alloc(sizeof(T) * new_capacity);

	if (m_size > 0)
	{
		memcpy(new_data, m_data, sizeof(T) * m_size);
	}

	m_allocator.free(m_data);
	m_data = new_data;
	m_capacity = new_capacity;
}
