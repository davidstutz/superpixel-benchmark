#pragma once

#include <tbb/scalable_allocator.h>
// No retry loop because we assume that scalable_malloc does
// all it takes to allocate the memory, so calling it repeatedly
// will not improve the situation at all
//
// No use of std::new_handler because it cannot be done in portable
// and thread-safe way (see sidebar)
//
// We throw std::bad_alloc( ) when scalable_malloc returns nullptr
//(we return nullptr if it is a no-throw implementation)
class custom_alloc
{
public:
	void* operator new (size_t size) throw (std::bad_alloc)
	{
		if (size == 0) size = 1;
		if (void* ptr = scalable_malloc (size))
		return ptr;
		throw std::bad_alloc ( );
	}

	void* operator new[] (size_t size) throw (std::bad_alloc)
	{
		return operator new (size);
	}

	void* operator new (size_t size, const std::nothrow_t&) throw ( )
	{
		if (size == 0) size = 1;
		if (void* ptr = scalable_malloc (size))
		return ptr;
		return nullptr;
	}

	void* operator new[] (size_t size, const std::nothrow_t&) throw ( )
	{
		return operator new (size, std::nothrow);
	}

	void operator delete (void* ptr) throw ( )
	{
		if (ptr != 0) scalable_free (ptr);
	}

	void operator delete[] (void* ptr) throw ( )
	{
		operator delete (ptr);
	}

	void operator delete (void* ptr, const std::nothrow_t&) throw ( )
	{
		if (ptr != 0) scalable_free (ptr);
	}

	void operator delete[] (void* ptr, const std::nothrow_t&) throw ( )
	{
		operator delete (ptr, std::nothrow);
	}
    
};

