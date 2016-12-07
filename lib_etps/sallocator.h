#pragma once

#include <string>

using namespace std;


// MemAllocator
///////////////////////////////////////////////////////////////////////////////

#define CLASS_ALLOCATION()\
    private:\
        static MemAllocator _allocator;\
    public:\
        void* operator new(size_t count) { return _allocator.Allocate(); }\
        void operator delete(void* object) { _allocator.Free(object); }
        
#define DEFINE_CLASS_ALLOCATOR(_class) MemAllocator _class::_allocator(sizeof(_class), 1024, #_class);
#define DEFINE_CLASS_ALLOCATOR_2(_class, _size) MemAllocator _class::_allocator(sizeof(_class), _size, #_class);


class MemAllocatorException : public exception {
};

class MemAllocator {
protected:
    struct InfoBlock {
        void* nextFree;
    };

    string name;        // name of the allocator, for debug purposes...
    void* memPool;      // pointer to memory pool
    void* freeSlot;     // pointer to free slot 
    size_t slotSize;    // size of slot (size of structure using this as its allocator)
    size_t slotCount;   // number of slots
    size_t memSize;     // size of memory
    size_t expandSize;  // number of slots added to the size of the pool when
                        // re-alloaction is needed. If 0, MemAllocatorException
                        // is thrown Not used (re-allocation not implemented)
public:
    MemAllocator(size_t slotSize_, size_t slotCount_, const string& name_);

	virtual ~MemAllocator();

	void* Allocate()
	{
        if (freeSlot == nullptr) throw MemAllocatorException();

        InfoBlock* ib = (InfoBlock*)freeSlot;
        if (ib->nextFree == nullptr) {
            throw MemAllocatorException(); 
            // extend to allow re-allocation
        } else {
            void* result = freeSlot;
            freeSlot = ib->nextFree;
            return result;
        }
    }

    void Free(void* p)
    {
        InfoBlock* ib = (InfoBlock*)p;
        ib->nextFree = freeSlot;
        freeSlot = p;
	}

protected:
    void InitMemPool();  
};


