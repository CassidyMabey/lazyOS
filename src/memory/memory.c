#include "memory.h"

#define HEAP_SIZE 1024*1024  // 1mb
#define BLOCK_SIZE sizeof(BlockHeader)

typedef struct BlockHeader {
    size_t size;
    uint8_t is_free;
    struct BlockHeader* next;
} BlockHeader;

static uint8_t heap[HEAP_SIZE];
static BlockHeader* head = NULL;

void init_memory(void) {
    head = (BlockHeader*)heap;
    head->size = HEAP_SIZE - BLOCK_SIZE;
    head->is_free = 1;
    head->next = NULL;
}

void* malloc(size_t size) {
    BlockHeader* current = head;
    BlockHeader* best_fit = NULL;
    size_t smallest_viable_size = HEAP_SIZE + 1;

    // best fit
    while (current) {
        if (current->is_free && current->size >= size) {
            if (current->size < smallest_viable_size) {
                best_fit = current;
                smallest_viable_size = current->size;
            }
        }
        current = current->next;
    }

    if (!best_fit) return NULL;

    // split if its too big
    if (best_fit->size > size + BLOCK_SIZE + 4) {
        BlockHeader* new_block = (BlockHeader*)((uint8_t*)best_fit + BLOCK_SIZE + size);
        new_block->size = best_fit->size - size - BLOCK_SIZE;
        new_block->is_free = 1;
        new_block->next = best_fit->next;
        
        best_fit->size = size;
        best_fit->next = new_block;
    }

    best_fit->is_free = 0;
    return (void*)((uint8_t*)best_fit + BLOCK_SIZE);
}

void free(void* ptr) {
    if (!ptr) return;

    BlockHeader* header = (BlockHeader*)((uint8_t*)ptr - BLOCK_SIZE);
    header->is_free = 1;

    // join it to teh next block
    if (header->next && header->next->is_free) {
        header->size += BLOCK_SIZE + header->next->size;
        header->next = header->next->next;
    }

    // previous block
    BlockHeader* current = head;
    while (current && current->next != header) {
        current = current->next;
    }

    if (current && current->is_free) {
        current->size += BLOCK_SIZE + header->size;
        current->next = header->next;
    }
}
