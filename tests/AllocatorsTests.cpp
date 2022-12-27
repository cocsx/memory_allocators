#include <gtest/gtest.h>

extern "C"
{
	#define ALLOCATORS_IMPL
	#include <memory_allocator/allocators.h>
}

#define NUM_OF_ELEMENTS 10

TEST(ArenaAllocator, arena_init)
{
	arena_s a;
	arena_init(&a, NUM_OF_ELEMENTS * sizeof(int));

	for (int i = 0; i < NUM_OF_ELEMENTS; ++i)
	{
		int* x = (int*) arena_alloc(&a, sizeof(int));
		*x = i;
	}

	for (int i = 0, j = 0; i < NUM_OF_ELEMENTS; i += DEFAULT_ALIGN, ++j)
		ASSERT_EQ(a.buf[i], j);

	arena_free(&a);
}

TEST(ArenaAllocator, arena_local_init)
{
	arena_s a;
	uint8_t buf[NUM_OF_ELEMENTS * DEFAULT_ALIGN * sizeof(uint8_t)] = {};
	arena_local_init(&a, buf, sizeof(buf));

	for (int i = 0; i < NUM_OF_ELEMENTS; ++i)
	{
		int* x = (int*) arena_alloc(&a, sizeof(int));
		*x = i;
	}

	for (int i = 0, j = 0; i < NUM_OF_ELEMENTS; i += DEFAULT_ALIGN, ++j)
		ASSERT_EQ(a.buf[i], j);

	arena_free(&a);
}