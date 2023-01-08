# Memory Allocators

An allocator allocate a block of memory (on the heap) and manages it, this make allocators more efficent than using malloc and free directly,
moreover the risk of not freeing a portion of memory is lower.

This library implements different kinds of allocators:
- Linear: Allocate memory linearly (the internal fragmentation is minimal); can only release the entire block of memory.
- Stack: Similar to the linear allocator but uses headers that permit to free and reuse the memory.
- Pool: Allocate memory that is divided in chunks of the same size. Each free chunk's pointer is stored in a linked list and when a chunk is allocated(/free) is removed(/added) to the linked list. The major downsize is that each chunk must be of the same size.

## Build and Integration in project

This project is an header only library so you can take [this file](./src/memory_allocators/allocators.h) and drop in your project or use cmake to import it.

