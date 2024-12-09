#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

/* If we haven't passed -DDEBUG=1 to gcc, then this will be set to 0: */
#ifndef DEBUG
#define DEBUG 0
#endif

#define LOGP(str) \
    do { if (DEBUG) fprintf(2, "%s:%d:%s(): %s", __FILE__, \
            __LINE__, __func__, str); } while (0)

#define LOG(fmt, ...) \
    do { if (DEBUG) fprintf(2, "%s:%d:%s(): " fmt, __FILE__, \
            __LINE__, __func__, __VA_ARGS__); } while (0)



struct __attribute__((__packed__)) mem_block {
    char name[8];
    uint64 size;
    struct mem_block *next_block;
    struct mem_block *prev_block;
};

struct __attribute__((__packed__)) free_block{
  struct mem_block block_header;
  struct free_block *next_free;
  struct free_block *prev_free;
};


uint current_fsm = FSM_FIRST_FIT;
bool scribble = false;

/**
 * Sets the memory allocation algorithm used by the system.
 * 
 * @param algorithm The algorithm identifier to set as the current memory allocation strategy.
 */
void malloc_setfsm(uint algorithm) {
    current_fsm = algorithm;
}

/**
 * Scribbles over a memory region with a predefined pattern to aid in debugging.
 * 
 * @param ptr A pointer to the memory region to scribble.
 * @param size The size of the memory region in bytes.
 */
void scribble_memory(void *ptr, uint size) {
    if (scribble && ptr) {
        memset(ptr, 0xAA, size);
    }
}

/**
 * Toggles the scribble mode used for debugging memory.
 * When enabled, allocated memory is filled with a pattern to detect uninitialized usage.
 */
void malloc_scribble() {
    scribble = !scribble;
}

/**
 * Marks a memory block as used.
 * 
 * @param block A pointer to the memory block to mark as used.
 */
void set_used(struct mem_block *block) {
    block->size &= ~0x01; // Clear the least significant bit to mark as used.
}

/**
 * Marks a memory block as free.
 * 
 * @param block A pointer to the memory block to mark as free.
 */
void set_free(struct mem_block *block) {
    block->size |= 0x01; // Set the least significant bit to mark as free.
}

/**
 * Checks if a memory block is free.
 * 
 * @param block A pointer to the memory block to check.
 * @return True if the block is free; false otherwise.
 */
bool is_free(struct mem_block *block) {
    return block->size & 0x01; // Check the least significant bit.
}

/**
 * Retrieves the size of a memory block, excluding the metadata.
 * 
 * @param block A pointer to the memory block to inspect.
 * @return The size of the memory block in bytes.
 */
uint64 get_size(struct mem_block *block) {
    return block->size & (~0x01);
}

struct mem_block *tail = NULL;
struct mem_block *head = NULL;
struct free_block *f_tail = NULL;
struct free_block *f_head = NULL;


/**
 * Adds a memory block to the doubly linked list of memory blocks.
 * 
 * @param block A pointer to the memory block to add.
 */
void add_memblock(struct mem_block *block) {
    if (head == NULL) {
        block->next_block = NULL;
        block->prev_block = NULL;
        head = block;
        tail = block;
    } else {
        tail->next_block = block;
        block->prev_block = tail;
        block->next_block = NULL;
        tail = block;
    }
}

/**
 * Adds a free block to the doubly linked list of free blocks.
 * 
 * @param block A pointer to the free block to add.
 */
void add_freeblock(struct free_block *block) {
    block->prev_free = NULL;

    if (f_head == NULL) {
        block->next_free = NULL;
        block->prev_free = NULL;
        f_head = block;
        f_tail = block;
    } else {
        f_tail->next_free = block;
        block->prev_free = f_tail;
        block->next_free = NULL;
        f_tail = block;
    }
    block->next_free = NULL;
}

/**
 * Checks if a free block exists in the doubly linked list of free blocks.
 * 
 * @param block A pointer to the free block to check.
 * @return True if the block is found; false otherwise.
 */
bool contains_freeblock(struct free_block *block) {
    struct free_block *current = f_head;
    while (current != NULL) {
        if (current == block) {
            return true;
        }
        current = current->next_free;
    }
    return false;
}

/**
 * Removes a memory block from the doubly linked list of memory blocks.
 * 
 * @param block A pointer to the memory block to remove.
 */
void remove_memblock(struct mem_block *block) {
    if (head == NULL) {
        return;
    }

    if (tail->prev_block == NULL) {
        head = NULL;
        tail = NULL;
        return;
    }

    if (tail == block) {
        tail = tail->prev_block;
        if (tail != NULL) {
            tail->next_block = NULL;
        }
        return;
    }
}

/**
 * Removes a free block from the doubly linked list of free blocks.
 * 
 * @param block A pointer to the free block to remove.
 */
void remove_freeblock(struct free_block *block) {
    if (f_head == NULL || block == NULL) {
        return;
    }

    if (block == f_head) {
        f_head = block->next_free;
        return;
    }

    if (block == f_tail) {
        f_tail = block->prev_free;
        return;
    }

    if (block->prev_free != NULL) {
        block->prev_free->next_free = block->next_free;
    }
    if (block->next_free != NULL) {
        block->next_free->prev_free = block->prev_free;
    }

    block->next_free = NULL;
    block->prev_free = NULL;
}

/**
 * Searches for and reuses a free memory block based on the specified size and the current
 * allocation strategy (First Fit, Best Fit, or Worst Fit).
 *
 * @param size The size of the memory block to allocate.
 * @return A pointer to a suitable memory block if available; otherwise, returns NULL.
 */
struct mem_block* reuse(uint64 size){
  struct free_block *free = f_head;
  struct free_block *best_fit = NULL;
  struct free_block *worst_fit = NULL;

  while(free != NULL){
    uint64 block_size = free->block_header.size & ~0x01;
    if(block_size >= size){
        switch(current_fsm){
          case FSM_FIRST_FIT:

            remove_freeblock(free);
            return (struct mem_block*)free;
          case FSM_BEST_FIT:
            if(best_fit == NULL || block_size < (best_fit->block_header.size & ~0x01)){
              best_fit = free;
            }
          case FSM_WORST_FIT:
            if(worst_fit == NULL || block_size > (worst_fit->block_header.size & ~0x01)){
              worst_fit = free;
            }
        }
    }
    free = free->next_free;
  }

  if(current_fsm == FSM_BEST_FIT && best_fit != NULL){
    remove_freeblock(best_fit);
    return (struct mem_block*)best_fit;
  }else if(current_fsm == FSM_WORST_FIT && worst_fit != NULL){
    remove_freeblock(worst_fit);
    return (struct mem_block*)worst_fit;
  }
  
  return NULL;
}

/**
 * Copies up to a specified number of characters from a source string to a destination string.
 * Ensures that the destination string is null-terminated.
 *
 * @param dest The destination string buffer.
 * @param src The source string to copy from.
 * @param size The maximum number of characters to copy (including the null terminator).
 */
void 
strncpy(char *dest, const char *src, uint64 size){
  uint64 i = 0;
  for(i = 0; i < size - 1 && src[i] != '\0'; i++){
    dest[i] = src[i];
  }
  dest[i] = '\0';
}

/**
 * Prints the current state of the memory allocation system, including:
 * - All memory blocks, their status (free or used), and names.
 * - The free list of available memory blocks.
 */
void malloc_print() {
    struct mem_block *current = head;
    printf("Current Memory State --\n");
    while (current != NULL) {
        char* status = is_free(current) ? "[FREE]" : "[USED]";
        void *block_end = (char *)current + (get_size(current));

        printf("[BLOCK %p-%p] %d   %s  '%s'\n", 
          (void *)current,               
          (void *)block_end,          
          get_size(current),             
          status,                      
          current->name);           

        current = current->next_block;
    }

    printf("-- Free List --\n");
    struct free_block *free = f_head;
    while(free != NULL){
      void *block_end = (char *)free;
      printf("[%p] -> ", block_end);
      free = free->next_free;
    }
    printf(" NULL\n");
}

/**
 * Checks for memory leaks in the allocation system by identifying allocated
 * memory blocks that are not freed.
 *
 * @return true if leaks are found (i.e., allocated blocks are not freed); otherwise, false.
 */
bool malloc_leaks(){
  struct mem_block *current = head;

  uint64 leaks = 0;
  uint64 bytes = 0;

  printf("-- Leak Check --\n");
  while(current != NULL){

    if(!is_free(current)){
      leaks++;
      bytes += current->size;

      printf("[BLOCK %p] %d\t%s\n", current, current->size, current->name);
    }
    current = current->next_block;
  }

  printf("-- Summary --\n");
  printf("%d blocks lost (%d bytes)\n", leaks, bytes);
  
  if(leaks > 0){
    return true;
  }

  return false;

}

/**
 * Merges adjacent free memory blocks into a single larger free block.
 * Updates the free list and memory block links accordingly.
 *
 * @param block The memory block to merge with adjacent free blocks.
 * @return A pointer to the newly merged free block.
 */
struct mem_block* merge_free(struct mem_block *block){
  struct mem_block *left = block->prev_block;
  struct mem_block *right = block->next_block;


  uint size = get_size(block);


  while(left != NULL && is_free(left)){
    size +=  get_size(left);
    remove_freeblock((struct free_block*) left);
    block = left;
    left = left->prev_block;
  }


  while(right != NULL && is_free(right)){
    size += get_size(right);
    remove_freeblock((struct free_block*) right);
    right = right->next_block;
  }


  if(left != NULL){
    left->next_block = block;
  }else{
    head = block;
  }

  if(right != NULL){
    right->prev_block = block;
  }else{
    tail = block;
  }

 

  block->size = size;
  block->prev_block = left;
  block->next_block = right;

  if(block->next_block == NULL){
    tail = block;
  }

  set_free(block);
  return block;
}

/**
 * Aligns a given size to the nearest multiple of the specified alignment.
 *
 * @param og The original size to align.
 * @param align The alignment boundary (e.g., 16, 4096).
 * @return The size aligned to the nearest multiple of `align`.
 */
uint64 align(uint64 og, uint64 align) {
    if (og % align == 0) {
        return og;
    }
    return ((og / align) + 1) * align;
}

/**
 * Frees a previously allocated memory block, merging adjacent free blocks if possible.
 * Deallocates memory back to the system if the block is the tail and its size is 4096 bytes or larger.
 *
 * @param ap A pointer to the memory block to free. Does nothing if the pointer is NULL.
 */
void free(void *ap) {
    if (ap == NULL) {
        return;
    }

    struct mem_block *block = ((struct mem_block *)ap) - 1;

    if (is_free(block)) {
        return;
    }

    block = merge_free(block);

    struct free_block *free_block = (struct free_block *)block;

    if (!contains_freeblock(free_block)) {
        add_freeblock(free_block);
    }

    if (block == tail && (block->size & ~0x01) >= 4096) {
        remove_memblock(block);
        remove_freeblock(free_block);

        uint64 total_sz = align(block->size & ~0x01, 4096);
        char *ret = sbrk(-total_sz);
        if (ret == (char *)-1) {
            LOG("sbrk deallocation failed! %p\n", ret);
        } else {
            LOG("De-allocating %d bytes! (new break: %p)\n", total_sz, ret);
        }
    }
}

/**
 * Splits a memory block into two smaller blocks if the remaining size is large enough
 * to accommodate a new memory block header and additional memory.
 *
 * @param block The memory block to split.
 * @param size The size of the first block after the split.
 * @param merge Flag to determine if the newly created block should be merged with adjacent free blocks.
 */
void split(struct mem_block *block, uint64 size, uint merge) {
    uint64 remain = get_size(block) - size - sizeof(struct mem_block);

    if (remain >= sizeof(struct mem_block) + 16) {
        struct mem_block *new_block = (struct mem_block *)((char *)block + size);

        new_block->size = remain + sizeof(struct mem_block);
        new_block->prev_block = block;
        new_block->next_block = block->next_block;

        if (block->next_block) {
            block->next_block->prev_block = new_block;
        }

        block->size = size;
        block->next_block = new_block;

        set_free(new_block);
        strncpy(new_block->name, "", sizeof(new_block->name));

        if (new_block->next_block == NULL) {
            tail = new_block;
        }

        add_freeblock((struct free_block *)new_block);
        if (merge == 1) {
            merge_free(new_block);
        }
    }
}

/**
 * Allocates a block of memory of at least the specified size.
 * If a suitable free block is found, it is reused; otherwise, memory is requested from the system.
 *
 * @param nbytes The size of the memory block to allocate.
 * @return A pointer to the allocated memory block, or NULL if allocation fails.
 */
void *malloc(uint nbytes) {
    uint64 total_sz = nbytes + sizeof(struct mem_block);
    total_sz = align(total_sz, 16);

    struct mem_block *reuse2 = reuse(total_sz);

    if (reuse2 != NULL) {
        if (get_size(reuse2) > total_sz + sizeof(struct mem_block)) {
            strncpy(reuse2->name, "", sizeof(reuse2->name));
            split(reuse2, total_sz, 0);
        }

        scribble_memory(reuse2 + 1, total_sz - sizeof(struct mem_block));
        set_used(reuse2);
        return reuse2 + 1;
    }

    uint full_size = align(total_sz, 4096);
    struct mem_block *block = (struct mem_block *)sbrk(full_size);

    if (block == (void *)-1) {
        return NULL;
    }

    block->size = full_size;

    set_used(block);
    add_memblock(block);

    if (block->size > total_sz + sizeof(struct free_block)) {
        split(block, total_sz, 0);
    }

    scribble_memory(block + 1, total_sz - sizeof(struct mem_block));
    return block + 1;
}

/**
 * Assigns a name to a memory block for identification during debugging or leak checks.
 *
 * @param ap A pointer to the allocated memory block.
 * @param name The name to assign to the memory block.
 */
void malloc_name(void *ap, const char *name) {
    struct mem_block *block = ((struct mem_block *)ap) - 1;
    strncpy(block->name, name, sizeof(block->name));
    LOG("Assigned name: %s for block %p\n", block->name, ap);
}

/**
 * Allocates a block of memory and initializes all its bytes to zero.
 *
 * @param nmemb The number of elements to allocate.
 * @param size The size of each element.
 * @return A pointer to the allocated and zeroed memory block, or NULL if allocation fails.
 */
void *calloc(uint64 nmemb, uint64 size) {
    void *memory = malloc(nmemb * size);
    if (memory == NULL) {
        return NULL;
    }

    memset(memory, 0, nmemb * size);
    return memory;
}

/**
 * Reallocates a memory block to a new size. If the block cannot be resized in place,
 * a new block is allocated, and the existing data is copied to the new block.
 *
 * @param ptr A pointer to the previously allocated memory block.
 *            If NULL, this function behaves like `malloc`.
 * @param size The new size for the memory block.
 *             If 0, the memory block is freed, and NULL is returned.
 * @return A pointer to the resized memory block, or NULL if allocation fails.
 */
void *realloc(void *ptr, uint64 size) {
    if (ptr == NULL) {
        return malloc(size);
    }

    if (size == 0) {
        free(ptr); 
        return NULL;
    }

    struct mem_block *block = ((struct mem_block *)ptr) - 1;
    uint64 og_size = get_size(block);

    if (og_size >= size + sizeof(struct mem_block)) {
        if (og_size - size >= sizeof(struct free_block) + 16) {
            size = align(size + sizeof(struct mem_block), 16);
            split(block, size, 1);
        }
        return ptr;
    }

    struct mem_block *merged = merge_free(block);

    size = align(size, 16);
    if ((merged->size & ~0x01) > size + sizeof(struct mem_block)) {
        size += sizeof(struct mem_block);

        if ((merged->size & ~0x01) > size + sizeof(struct free_block)) {
            split(merged, size, 0);
        }

        set_used(merged);
        scribble_memory(merged + 1, size - og_size);
        return merged + 1;
    }

    add_freeblock((struct free_block *)merged);
    void *new_block = malloc(size);

    if (new_block == NULL) {
        LOG("Memory Allocation failure!: %p", new_block);
        return NULL;
    }

    memcpy(new_block, ptr, og_size); 
    free(ptr); 
    return new_block;
}

