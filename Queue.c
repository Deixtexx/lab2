#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>


typedef struct PoolNode {
    struct PoolNode* next;
} PoolNode;

typedef struct {
    void* buffer;
    size_t block_size;
    PoolNode* free_list;
} PoolCtx;


typedef struct QueueNode {
    void* data;
    struct QueueNode* next;
} QueueNode;

typedef struct {
    QueueNode* head;
    QueueNode* tail;
    PoolCtx* pool; 
} Queue;


void pool_init(PoolCtx* ctx, void* buffer, size_t block_count, size_t block_size) {
    if (block_size < sizeof(PoolNode)) {
        block_size = sizeof(PoolNode);
    }

    ctx->buffer = buffer;
    ctx->block_size = block_size;
    ctx->free_list = (PoolNode*)buffer;

    PoolNode* current = ctx->free_list;
    for (size_t i = 0; i < block_count - 1; i++) {
        char* next_ptr = (char*)current + block_size;
        current->next = (PoolNode*)next_ptr;
        current = current->next;
    }
    current->next = NULL;
}

void* pool_alloc(PoolCtx* ctx) {
    if (ctx->free_list == NULL) return NULL;
    PoolNode* allocated_node = ctx->free_list;
    ctx->free_list = allocated_node->next;
    return (void*)allocated_node;
}

void pool_free(PoolCtx* ctx, void* ptr) {
    if (ptr == NULL) return;
    PoolNode* node_to_free = (PoolNode*)ptr;
    node_to_free->next = ctx->free_list;
    ctx->free_list = node_to_free;
}


void queue_init(Queue* q, PoolCtx* ctx) {
    q->head = NULL;
    q->tail = NULL;
    q->pool = ctx;
}

void queue_push(Queue* q, void* val) {
    QueueNode* n = (QueueNode*)pool_alloc(q->pool);

    if (!n) {
        printf("Ошибка: Пул пуст, невозможно добавить элемент!\n");
        return;
    }

    n->data = val;
    n->next = NULL;

    if (q->tail) {
        q->tail->next = n;
    }
    else {
        q->head = n;
    }
    q->tail = n;
}

void* queue_pop(Queue* q) {
    if (q->head == NULL) return NULL;

    QueueNode* temp = q->head;
    void* val = temp->data;

    q->head = q->head->next;
    if (q->head == NULL) {
        q->tail = NULL;
    }

    pool_free(q->pool, temp);

    return val;
}


int main() {
#define COUNT 3
#define BLOCK_SIZE sizeof(QueueNode)

    char memory[COUNT * BLOCK_SIZE];
    PoolCtx ctx;
    pool_init(&ctx, memory, COUNT, BLOCK_SIZE);

    Queue q;
    queue_init(&q, &ctx);

    int v1 = 10, v2 = 20, v3 = 30, v4 = 40;

    printf("Добавляем 3 элемента (заполняем пул)...\n");
    queue_push(&q, &v1);
    queue_push(&q, &v2);
    queue_push(&q, &v3);

    printf("Попытка добавить 4-й (должна быть ошибка):\n");
    queue_push(&q, &v4);

    printf("\nИзвлекаем элементы:\n");
    for (int i = 0; i < 3; i++) {
        int* val = (int*)queue_pop(&q);
        if (val) printf("Извлечено: %d\n", *val);
    }

    printf("\nПул снова свободен. Добавляем новый элемент (проверка переиспользования):\n");
    queue_push(&q, &v4);
    int* last = (int*)queue_pop(&q);
    if (last) printf("Извлечено: %d\n", *last);

    return 0;
}
