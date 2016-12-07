#include "stdafx.h"

#include <cstdlib>
#include <algorithm>
#include "sallocator.h"

// memory allocation
///////////////////////////////////////////////////////////////////////////////

// mem_allocator
//////////////////

MemAllocator::MemAllocator(size_t slotSize_, size_t slotCount_, const string& name_) :
    name(name_),
    memPool(nullptr), 
    slotSize(max(slotSize_, sizeof(void*))), 
    slotCount(max(slotCount_, (size_t)1))
{ 
    InitMemPool();
    freeSlot = memPool;
}

MemAllocator::~MemAllocator()
{
    free(memPool);
}

void MemAllocator::InitMemPool()
{
    memPool = malloc(slotCount*slotSize);

    char* ptr = (char*)memPool;

    for (int i = 1; i < slotCount; i++) {
        char* nextPtr = ptr + slotSize;
        ((InfoBlock*)ptr)->nextFree = nextPtr;
        ptr = nextPtr;
    }
    ((InfoBlock*)ptr)->nextFree = nullptr;
}


