#pragma once 

#ifndef ALLOCATORS_H
#define ALLOCATORS_H

#include <stdbool.h>
#include <stdlib.h>


typedef struct {
    void*  __buffer;
    size_t __buff_size;
    size_t __offset;
    bool   __heap;
} linear_s;

void  linear_init(linear_s* a, size_t size);
void* linear_alloc(linear_s* a, size_t size);
void  linear_release(linear_s* a);

typedef struct {
    void*  __buffer;
    size_t __buff_size; 
    size_t __offset;
    bool   __heap;
} stack_s;

void  stack_init(stack_s* a, size_t size);
void* stack_alloc(stack_s* a, uint size);
void  stack_free(stack_s* a);
void  stack_release(stack_s* a);


typedef struct {
    void*  __buffer;
    size_t __buff_size;
    bool   __heap;

    // Differences
    void*  __free_list_head;
    size_t __chunk_size;
} pool_s;

void  pool_init(pool_s * a, size_t size, size_t chunk_size);
void* pool_alloc(pool_s* a, size_t size);
void  pool_free(pool_s* a, void* ptr);
void  pool_release(pool_s* a);


#ifndef DEFAULT_ALIGN
    #define DEFAULT_ALIGN (2*sizeof(void *))
#endif

#ifdef ALLOCATORS_IMPLEMENTATION

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>


#define NOT_IMPLEMENTED(func_name) do {                         \
    assert(false && "[FATAL]:" #func_name "not implemented!");  \
} while(0)

#define FUNC_API extern

///////////////////////////////////////////////////////
/// Helper functions

static size_t __check_size(size_t sz)
{
    if (sz != 0 && !(sz & (sz - 1)))
        return sz;

    size_t out_size = 1;
    while(out_size < sz)
        out_size <<= 1;
    return out_size;
}

static uintptr_t __align(uintptr_t ptr, size_t align)
{
    uintptr_t aligned_ptr = ptr;
    size_t mod = (size_t) aligned_ptr & (align - 1);
    if (mod == 0)
        return aligned_ptr;
    aligned_ptr += (uintptr_t)(align - mod);
    return aligned_ptr;
}

static void __create_free_list(pool_s* a)
{
    size_t num_of_chunks = a->__buff_size / a->__chunk_size;
    for (size_t i = 0; i < num_of_chunks; ++i)
    {
        size_t chunk_offset = i * a->__chunk_size;
        size_t next_chunk_offset = ( i + 1 ) * a->__chunk_size;
        void* next_chunk_ptr = (void*)((size_t) a->__buffer + next_chunk_offset);
        if (next_chunk_ptr >= (void*)((size_t) a->__buffer + a->__chunk_size * num_of_chunks))
        {
            next_chunk_ptr = NULL;
        }
        void* chunk_ptr =  (void*)((size_t) a->__buffer + chunk_offset);
        memcpy(chunk_ptr, &next_chunk_ptr, sizeof(uintptr_t));
    }
    a->__free_list_head = a->__buffer;
}


///////////////////////////////////////////////////////
/// Linear allocator

FUNC_API void linear_init(linear_s* a, size_t size)
{
    a->__buff_size = __check_size(size);
    a->__buffer    = malloc(a->__buff_size);
    a->__offset    = 0;
    a->__heap    = true;
}

FUNC_API void* linear_alloc(linear_s* a, size_t size)
{
    uintptr_t curr_ptr  = (size_t) a->__buffer + a->__offset;
    size_t pad_offset   = __align(curr_ptr, DEFAULT_ALIGN) - (size_t) a->__buffer;

    if (pad_offset + size > a->__buff_size)
    {
        //printf("[ERROR]: Not enough space!");
        return NULL;
    }

    uintptr_t ptr = (size_t) a->__buffer + pad_offset;
    a->__offset = pad_offset + size;
    return (void*)ptr;
}

FUNC_API void linear_release(linear_s* a)
{
    if (a->__heap)
    {
        free(a->__buffer);
        a->__buffer = NULL;
        a->__buff_size = 0;
    }
    a->__offset = 0;
}


///////////////////////////////////////////////////////
/// Stack allocator

FUNC_API void stack_init(stack_s* a, size_t size)
{
    a->__buff_size = __check_size(size);
    a->__buffer    = malloc(a->__buff_size);
    a->__offset    = 0;
    a->__heap      = true;
}

FUNC_API void* stack_alloc(stack_s* a, uint size)
{ // NOTE: Add alignment
    if (a->__offset + size + sizeof(uint) > a->__buff_size)
    {
        printf("[ERROR]: Not enough space, can't allocate!");
        return NULL;
    }
    
    void* header_ptr = (void*) ( (size_t) a->__buffer + a->__offset + size);
    memcpy(header_ptr, &size, sizeof(uint));

    void* ptr = (void*) ( (size_t) a->__buffer + a->__offset);
    a->__offset = a->__offset + size + sizeof(uint);

    return ptr;
}

FUNC_API void stack_free(stack_s* a)
{
    uint block_size;
    void* ptr = (void*) ((size_t) a->__buffer + a->__offset - sizeof(uint));
    memcpy(&block_size, ptr, sizeof(uint));

    a->__offset -= block_size + sizeof(uint);
}

FUNC_API void stack_release(stack_s* a)
{
    if (a->__heap)
    {
        free(a->__buffer);
        a->__buffer = NULL;
        a->__buff_size = 0;
    }
    a->__offset = 0;
}


///////////////////////////////////////////////////////
/// Pool allocator

FUNC_API void pool_init(pool_s* a, size_t size, size_t chunk_size)
{ // NOTE: Change asserts with something more robust
    assert(chunk_size >= sizeof(uintptr_t) && "[FATAL]: The size of the chunk MUST be larger than `sizeof(uintptr_t)`!");
    assert(size > chunk_size && "[FATAL]: The buffer `size` must be bigger than the `chunk_size`!");
    assert(size % chunk_size == 0 && "[FATAL]: The value `size` must be a multiple of `chunk_size`!");

    a->__buff_size  = __check_size(size);
    a->__buffer     = malloc(a->__buff_size);
    a->__chunk_size = chunk_size;
    a->__heap     = true;
    __create_free_list(a);
}

FUNC_API void* pool_alloc(pool_s* a, size_t size)
{
    assert(size == a->__chunk_size && "[FATAL]: Can't allocate a chunk of a size that is different from the `chunk_size`!");
    void* ptr = a->__free_list_head;

    if (ptr == NULL)
    {
        printf("[ERROR]: No more chunks available!");
        return NULL;
    }
    
    void* curr_head = (void*) a->__free_list_head;
    void* next_head; 
    memcpy(&next_head, curr_head, sizeof(uintptr_t));

    a->__free_list_head = next_head;

    return ptr;
}

FUNC_API void pool_free(pool_s* a, void* ptr)
{
    assert( (((size_t) ptr - (size_t) a->__buffer)) % a->__chunk_size == 0 && "[FATAL]: To free a chunk you have to pass a valid ptr!");
    void* old_head = a->__free_list_head;
    
    // Change the head of the linked list O(1)
    memcpy(ptr, &old_head, sizeof(uintptr_t));
    a->__free_list_head = ptr;
}

FUNC_API void pool_release(pool_s* a)
{
    if (a->__heap)
    {
        free(a->__buffer);
        a->__buffer = NULL;
        a->__buff_size = 0;
    }
    a->__chunk_size     = 0;
    a->__free_list_head = NULL;
}


#endif /* ALLOCATORS_IMPLEMENTATION */

#endif /* ALLOCATORS_H */
