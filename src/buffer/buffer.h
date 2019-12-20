#ifndef LEO_BUFFER_H_
#define LEO_BUFFER_H_

#include <stddef.h>
#include <stdint.h>

typedef struct {
  size_t size;
  size_t capacity;
  char *data;
} Buffer;

Buffer buffer_new();
Buffer buffer_with_capacity(size_t capacity);
void buffer_clear(Buffer *buf);
void buffer_append(Buffer *buf, char *data, size_t len);
void buffer_append_str(Buffer *buf, char *data);

typedef struct {
  char *from;
  size_t size;
} Span;

#endif
