#include <gtest/gtest.h>

extern "C" {
    #include <limits.h>

    #define ALLOCATORS_IMPLEMENTATION
    #include <memory_allocators/allocators.h>
}

TEST(memory_allocator, linear_init_release)
{       
    linear_s a;
    linear_init(&a, 1024);
    ASSERT_EQ(0, a.__offset);
    ASSERT_EQ(true, a.__heap);
    ASSERT_NE(nullptr, a.__buffer);
    ASSERT_EQ(1024, a.__buff_size);

    linear_release(&a);
    ASSERT_EQ(0, a.__offset);
    ASSERT_EQ(nullptr, a.__buffer);
    ASSERT_EQ(0, a.__buff_size);
}

TEST(memory_allocator, linear_alloc)
{
    linear_s a;
    linear_init(&a, 32);

    {
        int* x = (int*)linear_alloc(&a, sizeof(int));
        ASSERT_NE(nullptr, x);
        ASSERT_EQ(4, a.__offset);
        *x = INT_MAX;

        x = (int*)linear_alloc(&a, sizeof(int));
        ASSERT_NE(nullptr, x);
        ASSERT_EQ(20, a.__offset);
        *x = INT_MAX;
    }

    {
        int* x = (int*)linear_alloc(&a, sizeof(int));
        ASSERT_EQ(nullptr, x);
    }

    linear_release(&a); 
}

TEST(memory_allocators, stack_init_release)
{ 
    stack_s a;
    stack_init(&a, 129);
    ASSERT_EQ(0, a.__offset);
    ASSERT_EQ(true, a.__heap);
    ASSERT_NE(nullptr, a.__buffer);
    ASSERT_EQ(256, a.__buff_size);

    stack_release(&a);
    ASSERT_EQ(0, a.__offset);
    ASSERT_EQ(nullptr, a.__buffer);
    ASSERT_EQ(0, a.__buff_size);
}

TEST(memory_allocators, stack_alloc_free)
{
    stack_s a;
    stack_init(&a, 16);

    {
        int* x = (int*)stack_alloc(&a, sizeof(int));
        ASSERT_NE(nullptr, x);
        ASSERT_EQ(8, a.__offset);
        *x = INT_MAX;

        x = (int*)stack_alloc(&a, sizeof(int));
        ASSERT_NE(nullptr, x);
        ASSERT_EQ(16, a.__offset);
        *x = INT_MAX;
    }

    {
        stack_free(&a);
        ASSERT_EQ(8, a.__offset);

        int* x = (int*)stack_alloc(&a, sizeof(int));
        ASSERT_NE(nullptr, x);
        ASSERT_EQ(16, a.__offset);
        *x = INT_MAX;
    }

    {
        int* x = (int*)stack_alloc(&a, sizeof(int));
        ASSERT_EQ(nullptr, x);
    }
    stack_release(&a);
}

TEST(memory_allocators, pool_init_release)
{
    pool_s a;
    pool_init(&a, 32, 16);
    ASSERT_EQ(true, a.__heap);
    ASSERT_EQ(32, a.__buff_size);
    ASSERT_EQ(16, a.__chunk_size);
    ASSERT_NE(nullptr, a.__buffer);
    ASSERT_NE(nullptr, a.__free_list_head);

    pool_release(&a);
    ASSERT_EQ(0, a.__buff_size);
    ASSERT_EQ(0, a.__chunk_size);
    ASSERT_EQ(nullptr, a.__buffer);
    ASSERT_EQ(nullptr, a.__free_list_head);
}

TEST(memory_allocators, pool_alloc_free)
{
    pool_s a;
    pool_init(&a, 16, 8);
    
    {
        void* prev_head = a.__free_list_head;
        long int* x = (long int*)pool_alloc(&a, sizeof(long int));
        ASSERT_NE(nullptr, x);
        ASSERT_NE(prev_head, a.__free_list_head);
        *x = LONG_MAX;

        x = (long int*) pool_alloc(&a, sizeof(long int));
        ASSERT_NE(nullptr, x);
        ASSERT_EQ(nullptr, a.__free_list_head);
        *x = LONG_MAX;
        
        pool_free(&a, x);
        ASSERT_NE(nullptr, a.__free_list_head);
        ASSERT_EQ((void*)x, a.__free_list_head);
    }

    pool_release(&a);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

