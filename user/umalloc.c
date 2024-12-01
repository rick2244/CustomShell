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
    /**
     * The name of this memory block. If the user doesn't specify a name for the
     * block, it should be left empty (a single null byte).
     */
    char name[8];

    /** Size of the block */
    uint64 size;

    /** Links for our doubly-linked list of blocks: */
    struct mem_block *next_block;
    struct mem_block *prev_block;
};

struct __attribute__((__packed__)) free_block{
  struct mem_block block_header;
  struct free_block *next_free;
  struct free_block *prev_free;
};


/*typedef enum {
    FSM_FIRST_FIT,
    FSM_BEST_FIT,
    FSM_WORST_FIT
} fsm_algo_t;*/

uint current_fsm = FSM_FIRST_FIT;

void 
malloc_setfsm(uint algorithm){
  current_fsm = algorithm;
  LOG("FSM algorithm set to: %d\n", current_fsm);
}

void set_used(struct mem_block *block){
  block->size &= ~0x01;
};

void set_free(struct mem_block *block){
  block->size |= 0x01;

};

bool is_free(struct mem_block *block){
  return block->size & 0x01;
};

uint64 get_size(struct mem_block *block){
  return block->size & (~0x01);
};

struct mem_block *tail = NULL;
struct mem_block *head = NULL;
struct free_block *f_tail = NULL;
struct free_block *f_head = NULL;

void add_memblock(struct mem_block *block){
  if(head == NULL){
    block->next_block = NULL;
    block->prev_block = NULL;
    head = block;
    tail = block;
  }else{
    tail->next_block = block;
    block->prev_block = tail;
    block->next_block = NULL;
    tail = block;
  }
}

void add_freeblock(struct free_block *block){

  block->next_free = NULL;
  block->prev_free = NULL;

  if(f_head == NULL){
    block->next_free = NULL;
    block->prev_free = NULL;
    f_head = block;
    f_tail = block;

  }else{
    f_tail->next_free = block;
    block->prev_free = f_tail;
    block->next_free = NULL;
    f_tail = block;
  }
}

bool contains_freeblock(struct free_block *block){
  struct free_block *current = f_head;
  while(current != NULL){
    if(current == block){
      return true;
    }
    current = current->next_free;
  }
  return false;
}

void add_inplace_memblock(struct mem_block *block){
  struct mem_block *current = head;
  while(current != NULL){
    if(current == block){
      struct mem_block *next = current->next_block;
      current->prev_block->next_block = block;
      block->next_block = next;
      break;
    }
    current = current->next_block;
  }
}

/*void add_memblock(struct mem_block *block) {
    // Ensure the block is properly initialized
    block->next_block = NULL;
    block->prev_block = NULL;

    if (head == NULL) {
        head = block;  
        tail = block;
    } else {
        struct mem_block *current = head;

        if ((char *)block < (char *)head) {
            // Insert at the start
            block->next_block = head;
            head->prev_block = block;
            head = block;
        } else {
            // Traverse the list to find the insertion point based on memory address
            while (current->next_block != NULL && (char *)current->next_block < (char *)block) {
                current = current->next_block;
            }

            printf("Current val: %p\n", current);
            printf("Block val: %p\n", block);
            printf("Nex val: %p\n", current->next_block);

            // Insert the new block after the current node
            block->next_block = current->next_block->next_block;
            block->prev_block = current;

            if (current->next_block != NULL) {
                // If we're inserting in the middle, update the previous pointer of the next block
                current->next_block->prev_block = block;
            }

            // Update the next pointer of the current block
            current->next_block = block;

            // If the block is inserted at the end, update the tail pointer
            if (block->next_block == NULL) {
                tail = block;
            }
        }
    }
}*/




void remove_memblock(struct mem_block *block){
  
  if(head == NULL){
    return;
  }

  if(tail->prev_block == NULL){
    head = NULL;
    tail = NULL;
    return;
  }

  if(tail == block){
    tail = tail->prev_block;
    if(tail != NULL){
       tail->next_block = NULL;
    } 
    return;
  }
  
}

void remove_freeblock(struct free_block *block) {
    if (f_head == NULL || block == NULL) {
        return;
    }

    if (block == f_head) {
        f_head = block->next_free;  
    }

    if (block == f_tail) {
        f_tail = block->prev_free; 
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

void 
strncpy(char *dest, const char *src, uint64 size){
  uint64 i = 0;
  for(i = 0; i < size - 1 && src[i] != '\0'; i++){
    dest[i] = src[i];
  }
  dest[i] = '\0';
}

void malloc_print() {
    struct mem_block *current = head;
    printf("Current Memory State --\n");
    while (current != NULL) {
        char* status = is_free(current) ? "[FREE]" : "[USED]";
        void *block_end = (char *)current + (current->size & ~0x01);

        // Print the memory state with proper address and size formatting
        printf("[BLOCK %p-%p] %d   %s  '%s'\n", 
          (void *)current,               // Format for the start address
          (void *)block_end,             // Format for the end address
          current->size & ~0x01,                 // Format for the size (integer)
          status,                        // Format for the status (string)
          current->name);                // Format for the name (string)

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


struct mem_block* merge_free(struct mem_block *block){
  //find the first block to the left that isn't free
  //struct mem_block *og = block;
  struct mem_block *left = block->prev_block;
  struct mem_block *right = block->next_block;


  uint size = block->size;

  uint left_cnt = 0;
  uint right_cnt = 0;

  while(left != NULL && is_free(left)){
    size +=  (left->size & ~0x01);
    remove_freeblock((struct free_block*) left);
    block = left;
    left_cnt++;
    left = left->prev_block;
  }
  while(right != NULL && is_free(right)){
    size += (right->size & ~0x01);
    remove_freeblock((struct free_block*) right);
    right_cnt++;
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

  set_free(block);
  LOG("Merged block: %p, new size: %d\n", (void *)block, size);
  return block;
}

uint64
align(uint64 og, uint64 align){

  if(og % align == 0){
    return og;
  }

  return ((og / align) + 1) * align;
}

void free(void *ap) {
    if (ap == NULL) {
        LOG("Attempted to free NULL pointer%p\n,", ap);
        return;
    }

    struct mem_block *block = ((struct mem_block *)ap) - 1;

    if (is_free(block)) {
        LOG("Double free detected for block %p\n", block);
        return;
    }

    // Merge free blocks
    //set_free(block); //need to set free

    block = merge_free(block);

    struct free_block *free_block = (struct free_block *)block;

    // Add to free list if not already present
    if (!contains_freeblock(free_block)) {
        add_freeblock(free_block);
    }

    // Check if we can release memory back to the system
    if (block == tail && (block->size & ~0x01) >= 4096) {
        remove_memblock(block);
        remove_freeblock(free_block);

        uint64 total_sz = align(block->size & ~0x01, 4096);
        LOG("Total size before deallocation: %d\n", block->size & ~0x01);
        char *ret = sbrk(-total_sz);
        if (ret == (char *)-1) {
            LOG("sbrk deallocation failed! %p\n", ret);
        } else {
            LOG("De-allocating %d bytes! (new break: %p)\n", total_sz, ret);
        }
    }
}


void 
split(struct mem_block *block, uint64 size){
  //malloc_print();
  //Things we need to check to determine if we can split:
  // - the block is not null
  // - the block is actually free
  // - the split size is greattter than our minimum size
  // - check if the block has enough free space (subtract out whatever the requested new size is and make sure we still have enough)
  
  // HINT: Use char * to do pointer arithmetic
  uint64 remain = (block->size & ~0x01) - size - sizeof(struct mem_block);
  LOG("Block size before split: %d, size: %d, sizeofstruct: %d\n", block->size, size, sizeof(struct mem_block));
  //only negative sbrk if it is a full page of data
  if(remain >= sizeof(struct mem_block)){
    // Calculate the memory address for the new block
        struct mem_block *new_block = (struct mem_block *)((char *)block  + size);

        // Initialize the new block
        new_block->size = remain + sizeof(struct mem_block);
        new_block->prev_block = block;  // Link the new block back to the original block
        new_block->next_block = block->next_block;  // Link the new block to the next block in the memory list


        if (block->next_block) {
            block->next_block->prev_block = new_block;  // Update the next block's previous pointer
        }

        // Update the original block to reflect the split
        //block->size = size;
        block->size = size & ~0x01;
        block->next_block = new_block;

        set_free(new_block);

        if(new_block->next_block == NULL){
          tail = new_block; //if this new addition is the tail than set it as the new tail
        }
        // Add the new block to the free list
        add_freeblock((struct free_block *)new_block);

        // Debugging information
        LOG("Split block at %p into:\n", (void *)block);
        LOG("- Block 1: %p (size: %d)\n", (void *)block, size);
        LOG("- Block 2: %p (size: %d)\n", (void *)new_block, remain);
  }
}

void *
malloc(uint nbytes)
{
  //make sure to aligh allocations to 16 bytes!
  uint64 total_sz = nbytes + sizeof(struct mem_block);
  LOG("Allocation request: %d; total size %d\n", nbytes, total_sz);

  //one char = 1 byte, so sbrk returna char *
  //todo, try to reuse an existing allocator (blocks set to free)
  //ex: reuse(size) -> a block, or NULL if we couldn't find one to reuse
  //FSM algorithms
  //if we can reuse, we SPLIT IT FIRST (if possible), we return it and stop here!

  //If we couldn't reuse a block, proceed
  //If we need to sbrk more data, we do it on multiples of the page size (4096)
  //this means that we will have to *split* blocks
  //Splitting:
  // -use the first part of the sbrk'd data for the block that the user asked for
  // -use the remainder of the free block that can be reused later

 
  total_sz = align(total_sz, 16);

  struct mem_block *reuse2 = reuse(total_sz);

  if(reuse2 != NULL){
      LOG("Split is being attempted! %p\n", reuse2);
      set_used(reuse2);
      if(reuse2->size > total_sz + sizeof(struct mem_block)){//means that we can split
      
        strncpy(reuse2->name, "", sizeof(reuse2->name));
      
        //printf("Size of new block: %d, location: %p\n", reuse2->size, reuse2);
        split(reuse2, total_sz);
        reuse2->size = total_sz;
      }
      return (struct mem_block *)((char *)reuse2 + sizeof(struct mem_block));
  }
  
  //every time I sbrk I need to align by 4096
  uint full_size = align(total_sz, 4096);

  struct mem_block *block = (struct mem_block *) sbrk(full_size);

  if(block == (void *) - 1){
    LOG("Memory allocation failure!: %p\n", block);
    return NULL;
  }


  //this should be a function! (and other linked list related stuff)
  block->size = full_size;

  set_used(block);
  add_memblock(block);

  LOG("Allocation successful @ %p, size: %d\n", block, block->size);

  if (block->size > total_sz + sizeof(struct mem_block)) {
      split(block, total_sz);
  }

  return block + 1;
}

void 
malloc_name(void *ap, const char *name){
  struct mem_block *block = ((struct mem_block *) ap) - 1;
  strncpy(block->name, name, sizeof(block->name));

  LOG("Assigned name: %s for block %p\n", block->name, ap);
}

void *
calloc(uint64 nmemb, uint64 size){
  //malloc but zeroed out 
  void *memory = malloc(nmemb * size);
  if(memory == NULL){
    return NULL;
  }

  memset(memory, 0, nmemb * size);
  return memory;
}

void *
realloc(void *ptr, uint64 size){

  struct mem_block *block = ((struct mem_block *)ptr) - 1;

  if(block == NULL){
    malloc(NULL);
  }
  if(size == 0){
    free(ptr);
  }
  
}