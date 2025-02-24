#include <cstdio>

#include "Array.h"
#include "SwissTable.h"
#include "ScratchAllocator.h"
#include "Timer.h"

struct Test
{
	const char* name;
	int health;
};

//constexpr const char* test_string = "kort strong";
constexpr const char* test_string = "long streng som moste allokere minne";


auto fill_swiss(int n)
{
	SwissTable<Test> hash;

	for (int i = 0; i < n;++i)
	{
		hash.insert(i, Test{test_string, i});
	}

	return hash;
}

auto accumulate_swiss(const SwissTable<Test>& in)
{
	u64 sum = 0;
	for (u32 i = 0; i < in.capacity; ++i)
	{
		if (in.control[i] != DELETED && in.control[i] != EMPTY)
		{
			auto& v = in.data[i];
			sum += v.value.health + strlen(v.value.name);
		}
	}
	return sum;
}

	
auto accumulate_swiss_rand(SwissTable<Test>& in)
{
	u64 sum = 0;

	srand(0);

	for (int i = 0; i < 1000000; ++i)
	{
		sum += in.find(rand()*10)->health;
	}
	
	return sum;
}

void copy_swiss(SwissTable<Test>& in)
{
	SwissTable<Test> v;
	for (int i = 0; i < 1; ++i)
	{
		v = in;
	}

	in = v;
}

bool testSwissTable() {
    SwissTable<int> table;

    // Insert and Find Test
    table.insert(1, 100);
    table.insert(2, 200);
    table.insert(3, 300);

    if (!table.find(1) || *table.find(1) != 100) return false;
    if (!table.find(2) || *table.find(2) != 200) return false;
    if (!table.find(3) || *table.find(3) != 300) return false;

    // Overwrite Existing Key
    table.insert_or_assign(2, 250);
    if (!table.find(2) || *table.find(2) != 250) return false;

    // Find Non-Existent Key
    if (table.find(99) != nullptr) return false;

    // Erase Key and Check
    table.erase(2);
    if (table.find(2) != nullptr) return false;

    // Reinsert After Erase
    table.insert(2, 500);
    if (!table.find(2) || *table.find(2) != 500) return false;

    // Bulk Insert and Find
    for (u64 i = 10; i < 30; ++i) {
        table.insert(i, static_cast<int>(i * 10));
    }
    for (u64 i = 10; i < 30; ++i) {
        if (!table.find(i) || *table.find(i) != static_cast<int>(i * 10)) return false;
    }

    // Bulk Erase and Check
    for (u64 i = 10; i < 20; ++i) {
        table.erase(i);
    }
    for (u64 i = 10; i < 20; ++i) {
        if (table.find(i) != nullptr) return false;
    }
    for (u64 i = 20; i < 30; ++i) {
        if (!table.find(i) || *table.find(i) != static_cast<int>(i * 10)) return false;
    }

    return true;
}

bool testArray(Allocator& a)
{
	Array<int> arr(a);
	arr.push_back(1);
	arr.push_back(2);
	arr.push_back(3);

	if (arr[0] != 1) return false;
	if (arr[1] != 2) return false;
	if (arr[2] != 3) return false;

	arr.push_back(4);
	arr.push_back(5);
	arr.push_back(6);

	if (arr[3] != 4) return false;
	if (arr[4] != 5) return false;
	if (arr[5] != 6) return false;

	arr.resize(3);

	if (arr[0] != 1) return false;
	if (arr[1] != 2) return false;
	if (arr[2] != 3) return false;

	arr.reserve(10);

	if (arr[0] != 1) return false;
	if (arr[1] != 2) return false;
	if (arr[2] != 3) return false;


	int c = 1;
	for(int i : arr)
	{
		if (i != c) return false;
		++c;
	}

	if(c != 4) return false;

	arr.clear();

	for(int i = 0; i < 1000; ++i)
	{
		arr.push_back(i);
	}

	if(arr.size() != 1000) return false;

	int sum = 0;
	for(int i : arr)
	{
		sum += i;
	}

	if(sum != 499500) return false;

	if(arr.size() != 0) return false;

	return true;
}

void fill_array_and_sum(Array<Array<char>>& arr, Allocator& a)
{
	for (int i = 0; i < 100000; ++i)
	{
		arr.push_back(Array<char>(a));

		auto& ins = arr.back();

		for (int ii = 0; ii < rand() % 1000; ++ii)
		{
			ins.push_back('a' + ii);
		}
	}
}



int main()
{
	MallocAllocator ma;
	puts("Init memory");
	block_memory_init();
	{
		Timer t;
		timer_init(&t);

		
		for (int i = 0; i < 1; ++i)
		{
			timer_start(&t);
			{
				Array<Array<char>> arra(ma);
				fill_array_and_sum(arra, ma);
			}
			timer_elapsed_ms(&t);
			printf("Fill array malloc took %f ms\n", timer_elapsed_ms(&t));

			timer_start(&t);
			{
				ScratchPadAllocator sa;
				void* t1 = sa.alloc(Block::BLOCK_SIZE / 2);
				void* t2 = sa.alloc(Block::BLOCK_SIZE / 2);
				/*using Str = Array<Array<char>>;
				Array<Array<char>>* arrb = new(sa.alloc(sizeof(Str))) Str(sa);
				fill_array_and_sum(*arrb, sa);*/
			}
			timer_elapsed_ms(&t);
			printf("Fill array scratch took %f ms\n", timer_elapsed_ms(&t));
		}
	}

	puts("shutting down memory");
	block_memory_shutdown();
	puts("memory shutdown");
}


