#ifndef ALLOCATORS_H
#define ALLOCATORS_H

#include <stdint.h>

#ifndef DEFAULT_ALIGN
	#define DEFAULT_ALIGN (2*sizeof(void *))
#endif

typedef struct arena_st {
	uint8_t* buf;
	size_t buf_len;
	size_t offset;

	bool _local;
} arena_s;


// Allocate the specified number_of_bytes on the heap.
void arena_init(arena_s* a, size_t number_of_bytes);

// Take a buffer that it uses as a region of memory that manages.
// When using this initialization the size of the buffer have to be
// calculated considering the DEFAULT_ALIGN variable.
// Example:
//     uint8_t buf[SIZE * DEFAULT_ALIGN * sizeof(uint8_t)]
void arena_local_init(arena_s* a, uint8_t* buf, size_t buf_size);

// Allocate on the internal buffer the size in input, if
// there isn't space the function return NULL.
void* arena_alloc(arena_s* a, size_t size);

// Free the space allocated all at once
void arena_free(arena_s* a);

#ifdef ALLOCATORS_IMPL
#define FUNC_API inline

#include <stdbool.h>

static size_t align(size_t ptr, size_t align)
{
	size_t aligned_ptr = ptr;
	size_t mod = aligned_ptr & (align - 1);
	if (mod == 0)
		return aligned_ptr;
	aligned_ptr += align - mod;
	return aligned_ptr;
}

static size_t check_size(size_t size)
{
	if (size != 0 && !(size & (size - 1)))
		return size;
	size_t k = 1;
	while(k < size)
		k <<= 1;
	return k;
}

FUNC_API void arena_init(arena_s* a, size_t number_of_bytes)
{
	number_of_bytes = check_size(number_of_bytes * DEFAULT_ALIGN);
	a->buf          = (uint8_t*) malloc(number_of_bytes);
	a->buf_len      = number_of_bytes;
	a->offset       = 0;
	a->_local       = false;
}

FUNC_API void arena_local_init(arena_s* a, uint8_t* buf, size_t buf_size)
{
	a->buf      = buf;
	a->buf_len  = buf_size;
	a->offset   = 0;
	a->_local   = true;
}

FUNC_API void* arena_alloc(arena_s* a, size_t size)
{
	if (a->buf_len == 0) {
		printf("[ERROR]: Arena must be initialized before calling this method!");
		return NULL;
	}


	uintptr_t curr_ptr   = (uintptr_t) &a->buf[a->offset];
	size_t padded_offset = align(curr_ptr, DEFAULT_ALIGN) - (size_t) a->buf;

	if (padded_offset + size > a->buf_len) {
		printf("[ERROR]: Not enough space!");
		return NULL;
	}

	void* ptr = &a->buf[padded_offset];
	a->offset = padded_offset + size;
	return ptr;
}

FUNC_API void arena_free(arena_s* a)
{
	if (!a->_local) // If the memory is allocated on the heap
	{
		free(a->buf);
		a->buf = NULL;
		a->buf_len = 0;
	}
	a->offset = 0;
}

#endif /* ALLOCATORS_IMPL */

#endif /* ALLOCATORS_H */