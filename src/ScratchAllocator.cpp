#include "ScratchAllocator.h"

#include <cstdio>

static Block* s_freeBlocks = nullptr;

void return_block(Block* block)
{
    Block* last = block;
    
    for(;;)
    {
        if(last->header.prev)
        {
            last = last->header.prev;
        }
        else
        {
            break;
        }
    }

    last->header.prev = s_freeBlocks;
    s_freeBlocks = block;
}

static Block* dbg_block[32];

Block* get_block()
{
    Block* block = s_freeBlocks;
    if(block)
    {
        s_freeBlocks = block->header.prev;
        block->header.prev = nullptr;
        return block;
    }
    else 
    {
        block = (Block*)::malloc(sizeof(Block));
        block->header.prev = nullptr;
        return block;
    }
}

void block_memory_init()
{
    for(i32 i = 0; i < 32; i++)
    {
        Block* block = (Block*)::malloc(sizeof(Block));
        memset(block, 0, sizeof(Block));
        block->header.prev = s_freeBlocks;
        s_freeBlocks = block;
        dbg_block[i] = block;
    }
}

void block_memory_shutdown()
{
    i32 blocks_freed = 0;
    Block* block = s_freeBlocks;
    while(block)
    {
        Block* next = block->header.prev;
        ::free(block);
        ++blocks_freed;
        block = next;
    }
		
    printf("Blocks freed %d\n", blocks_freed);
}

ScratchPadAllocator::ScratchPadAllocator()
{
    m_current = get_block();
    m_pos = 0;
}

ScratchPadAllocator::~ScratchPadAllocator()
{
    return_block(m_current);
}

void* ScratchPadAllocator::alloc(i32 size)
{
   i32 size_with_alignment = size + 16 - (size & 15);

    if(size_with_alignment > Block::BLOCK_SIZE)
    {
        __debugbreak();
        return nullptr;
    }

    if(size_with_alignment + m_pos > Block::BLOCK_SIZE)
    {
        Block* next = get_block();
        next->header.prev = m_current;
        m_current = next;
        m_pos = 0;
        return alloc(size);
    }
    else
    {
        i32 pos = m_pos;
        m_pos += size_with_alignment;
        return (void*)&m_current->data[pos];
    }
}

void ScratchPadAllocator::free(void* data, i32 size)
{
    // Do nothing
}