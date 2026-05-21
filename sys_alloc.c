#include "allocator.h"
#include <stddef.h>
#include <stdlib.h>

void *sys_alloc(IAllocator *self, size_t size) { return malloc(size); }

void sys_free(IAllocator *self, void *ptr) { free(ptr); }

void *sys_realloc(IAllocator *self, void *ptr, size_t new_size) {
  return realloc(ptr, new_size);
}

IAllocator create_sys_alloc() {
  IAllocator allocator;
  allocator.alloc = sys_alloc;
  allocator.free = sys_free;
  allocator.realloc = sys_realloc;
  allocator.reset = stub_reset;
  return allocator;
}

int main(void) {}