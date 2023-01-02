#include <gtest/gtest.h>

extern "C" {
    #include <limits.h>

    #define ALLOCATORS_IMPLEMENTATION
    #include <memory_allocators/allocators.h>
}

TEST(memory_allocator, linear_init_release_tests)
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

TEST(memory_allocator, linear_alloc_tests)
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

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
