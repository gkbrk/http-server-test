#include <stdlib.h>
#include <string.h>

#include "buffer.h"

void buffer_append(Buffer *buf, char *data, size_t len) {
  if (buf->size + len > buf->capacity) {
    buf->capacity *= 2;
    buf->data = realloc(buf->data, buf->capacity);
  }

  memcpy(buf->data + buf->size, data, len);
  buf->size += len;
}
