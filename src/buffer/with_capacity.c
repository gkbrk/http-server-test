#include <stdlib.h>

#include "buffer.h"

Buffer buffer_with_capacity(size_t capacity) {
  Buffer b = buffer_new();
  char *buf = malloc(capacity);
  if (buf != NULL) {
    b.capacity = capacity;
    b.data = buf;
  }
  return b;
}
