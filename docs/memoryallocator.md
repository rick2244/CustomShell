Allocating Memory

Memory allocations must be aligned to 16 bytes; in other words, the size of the memory blocks should be evenly divisible by 16. The minimum viable block’s data portion is 16 bytes, and the overall minimum size of a block is 48 bytes.

Once basic allocation works, you can start splitting blocks that are not 100% used. For instance, if a block is 4096 bytes in size but only 96 bytes are actually used, split the block in two: one 96-byte block, and one 4000-byte block.

When implementing your free space management algorithms, ties (i.e., blocks that satisfy the algorithm and are the same size) should be broken by choosing the first allocation you found based on the linked list order.

Freeing Memory:

First, set the 0th bit of size to 1. Next, use the data payload portion of the block to store a pointer to the next free block. That’s it! This approach is why you sometimes can read ‘old’ values from memory that have been freed. After freeing a block, you should also check neighboring blocks to determine whether you can merge with them or not. Merge with any free neighboring blocks.

If an entire block has been freed (i.e., 4096 bytes or more are free at the end of the address space), then you should decrease the bound of the program with sbrk to release the memory for other programs to use.


Reallocating Memory
If the user wants to realloc a pointer, first check to see if the block can be resized in place. Ways this could happen:

The block already has some extra space because of its alignment, so no changes need to be made to complete the realloc
The block is being shrunk, requiring (1) a metadata update, and (2) splitting off a new block from the extra free space that was made (if possible).
If the free space created by shrinking a block is located next to another free block, they should be merged.
The block can expand into a neighboring free block. If the entire free block is not consumed by the expansion, then the remaining free space should be split off into a new free block.
If none of the situations above are possible (e.g., the block is too large to resize in place), simply malloc a new, appropriately sized block, copy the data there, and then free the old block.

Edge Cases: If the pointer passed into realloc is NULL, then it should behave like malloc instead since there is nothing to resize. Additionally, if the size passed into realloc is 0, then the block should be freed.


Memory State Information: your allocator should be able to print out the current memory state with the malloc_print() function. See the format below.

-- Current Memory State --
[BLOCK 0x7f0d774e7000-0x7f0d774e70a8] 168     [USED]  'Blk 1'
[BLOCK 0x7f0d774b0000-0x7f0d774b0050] 80      [USED]  'Blk 2'
[BLOCK 0x7f0d774af000-0x7f0d774af0a8] 168     [USED]  'Blk 3'
     ...
(list continues)

-- Free List --
[0x7f0d774e70a8] -> [0x7f0d774b0050] -> [0x7f0d774af0a8] -> (...) -> NULL

Leak Check: You can leverage the metadata we are tracking to find memory leaks, so add a malloc_leaks() function. malloc_leaks() will print leaks, a summary, and return true if leaks were found:

-- Leak Check --
[BLOCK 0x7f0d774e7000] 168     'Blk 1'
[BLOCK 0x7f0d774b0000] 80      'Blk 2'
     ...
  (list continues)

-- Summary --
542 blocks lost (892412 bytes)

