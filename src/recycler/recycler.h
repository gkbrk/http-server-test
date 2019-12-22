#ifndef RECYCLER_H_
#define RECYCLER_H_

typedef void *(recycler_alloc)(void);
typedef void (recycler_free)(void *obj);

typedef struct {
  size_t size;
  size_t capacity;
  recycler_alloc *alloc;
  recycler_free *free;
  void **objs;
} Recycler;

Recycler recycler_new(recycler_alloc *alloc, recycler_free *free);
void *recycler_get(Recycler *recycler);
void recycler_recycle(Recycler *recycler, void *obj);

#endif
