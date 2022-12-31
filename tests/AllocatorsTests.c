#define ALLOCATORS_IMPLEMENTATION
#include <memory_allocators/allocators.h>

#include <limits.h>

int main(int argc, char** argv)
{
    (void)argc; (void)argv;
    {
        linear_s a;
        linear_init(&a, 1024);

        int* x = (int*) linear_alloc(&a, sizeof(int));
        *x = INT_MAX;

        x = (int*) linear_alloc(&a, sizeof(int));
        *x = INT_MAX;

        linear_release(&a);
    }

    {
        stack_s a;
        stack_init(&a, 32);

        int* x = (int*) stack_alloc(&a, sizeof(int));
        *x = INT_MAX;

        long int* y = (long int*) stack_alloc(&a, sizeof(long int));
        *y = LONG_MAX;

        stack_free(&a);
        stack_free(&a);


        stack_release(&a);
    }

    {
        pool_s a;
        pool_init(&a, 32, 16);

        int* x = (int*) pool_alloc(&a, 4 * sizeof(int));
        *x = INT_MAX;

        int* y = (int*) pool_alloc(&a, 4 * sizeof(int));
        *y = INT_MAX;

        pool_free(&a, x);

        int* z = (int*) pool_alloc(&a, 4 * sizeof(int));
        *z = INT_MAX;

        pool_free(&a, z);
        pool_free(&a, y);

        pool_release(&a);
    }

    return 0;
}
