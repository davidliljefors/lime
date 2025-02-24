#include "Array.h"
#include "Core.h"

struct Block
{
	static constexpr i32 PAGE_SIZE = 4096;
	// 4 megabyte blocks
	static constexpr i32 BLOCK_SIZE = 16 * 256 * PAGE_SIZE;
	struct Header
	{
		Block* prev;
	};

	Header header;

	u8 data[BLOCK_SIZE-sizeof(Header)];
};

void block_memory_init();
void block_memory_shutdown();

struct ScratchPadAllocator : public Allocator
{
	ScratchPadAllocator();
	~ScratchPadAllocator() override;

	ScratchPadAllocator(const ScratchPadAllocator&) = delete;
	ScratchPadAllocator(ScratchPadAllocator&&) = delete;

	ScratchPadAllocator& operator=(const ScratchPadAllocator&) = delete;
	ScratchPadAllocator& operator=(ScratchPadAllocator&&) = delete;

	void* alloc(i32 size) override;
	void free(void* block, i32 size) override;
private:
	Block* m_current;
	i32 m_pos;
};
