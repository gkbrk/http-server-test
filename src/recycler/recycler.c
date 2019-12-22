#include <stdlib.h>

#include "recycler.h"



Recycler recycler_new(recycler_alloc *alloc, recycler_free *free) {
  Recycler r;
  r.size = 0;
  r.capacity = 0;
  r.alloc = alloc;
  r.free = free;
  r.objs = NULL;
  return r;
}

void *recycler_get(Recycler *recycler) {
  if (recycler->size > 0) {
    void *obj = recycler->objs[recycler->size - 1];
    recycler->size--;
    return obj;
  }

  return recycler->alloc();
}

void recycler_recycle(Recycler *recycler, void *obj) {
  if (recycler->size == recycler->capacity) {
    recycler->capacity += 10;
    recycler->objs = realloc(recycler->objs,
                             recycler->capacity * sizeof(void*));
  }

  recycler->objs[recycler->size] = obj;
  recycler->size++;
}
