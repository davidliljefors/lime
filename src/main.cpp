#include "Array.h"
#include "SwissTable.h"

struct Test
{
	const char* name;
	int health;
};

//constexpr const char* test_string = "kort strong";
constexpr const char* test_string = "long streng som moste allokere minne";


// auto fill_hash(int n)
// {
// 	hashtable::Hashtable<Test> hash;

// 	for (int i = 0; i < n;++i)
// 	{
// 		hashtable::insert(hash, i, Test{test_string, i});
// 	}

// 	return hash;
// }



// auto fill_unordered(int n)
// {
// 	std::unordered_map<u64, Test> hash;

// 	for (int i = 0; i < n;++i)
// 	{
// 		hash[i] = Test{test_string, i};
// 	}

// 	return hash;
// }


auto fill_swiss(int n)
{
	SwissTable<Test> hash;

	for (int i = 0; i < n;++i)
	{
		hash.insert(i, Test{test_string, i});
	}

	return hash;
}

// auto accumulate_hash(const hashtable::Hashtable<Test>& in)
// {
// 	u64 sum = 0;
// 	for (auto& v : in.data)
// 	{
// 		sum += v.value.health + strlen(v.value.name.c_str());;
// 	}
// 	return sum;
// }

// auto accumulate_unordered(const std::unordered_map<u64, Test>& in)
// {
// 	u64 sum = 0;
// 	for (auto& v : in)
// 	{
// 		sum += v.second.health + strlen(v.second.name.c_str());
// 	}
// 	return sum;
// }

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

// auto accumulate_unordered_rand(const std::unordered_map<u64, Test>& in)
// {
// 	u64 sum = 0;

// 	srand(0);

// 	for (int i = 0; i < 1000000; ++i)
// 	{
// 		sum += in.find(rand()*10)->second.health;
// 	}
	
// 	return sum;
// }

// auto accumulate_hash_rand(hashtable::Hashtable<Test>& in)
// {
// 	u64 sum = 0;

// 	srand(0);

// 	for (int i = 0; i < 1000000; ++i)
// 	{
// 		sum += in.data[findImpl(in, rand()*10).dataIndex].value.health;
// 	}
	
// 	return sum;
// }

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


// void copy_unordered(std::unordered_map<u64, Test>& in)
// {
// 	std::unordered_map<u64, Test> v;
// 	for (int i = 0; i < 1; ++i)
// 	{
// 		v = in;
// 	}

// 	in = v;
// }


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

void flush()
{
	int cache_sz = 200000000;
	char* mem = (char*)malloc(cache_sz);
	memset(mem, 1, cache_sz);

	free(mem);
}

int main()
{
	MallocAllocator ma;
	testSwissTable();
	testArray(ma);


}


