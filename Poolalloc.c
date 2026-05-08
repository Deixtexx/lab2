#include <stdio.h>
#include <stddef.h>

typedef struct Node {
    struct Node* next;
} Node;

typedef struct {
    void* buffer;
    size_t block_size;
    Node* free_list;
} PoolCtx;


void pool_init(PoolCtx* ctx, void* buffer, size_t block_count, size_t block_size) {
    if (block_size < sizeof(Node)) {
        block_size = sizeof(Node);
    }

    ctx->buffer = buffer;
    ctx->block_size = block_size;
    ctx->free_list = (Node*)buffer;

    Node* current = ctx->free_list;
    for (size_t i = 0; i < block_count - 1; i++) {
        char* next_ptr = (char*)current + block_size;
        current->next = (Node*)next_ptr;
        current = current->next;
    }
    current->next = NULL;
}

void* pool_alloc(PoolCtx* ctx) {
    if (ctx->free_list == NULL) return NULL;
    Node* allocated_node = ctx->free_list;
    ctx->free_list = allocated_node->next;
    return (void*)allocated_node;
}

void pool_free(PoolCtx* ctx, void* ptr) {
    if (ptr == NULL) return;
    Node* node_to_free = (Node*)ptr;
    node_to_free->next = ctx->free_list;
    ctx->free_list = node_to_free;
}


int main() {
#define POOL_SIZE 3

#define BLOCK_SIZE (sizeof(int) > sizeof(Node) ? sizeof(int) : sizeof(Node))

    char memory_pool[POOL_SIZE * BLOCK_SIZE];

    PoolCtx ctx;
    pool_init(&ctx, memory_pool, POOL_SIZE, BLOCK_SIZE);

    int* a = (int*)pool_alloc(&ctx);
    if (a) {
        *a = 42;
        printf("Значение а: %d\n", *a);
    }

    pool_free(&ctx, a);
    printf("Блок успешно освобождён.\n");

    return 0;
}
