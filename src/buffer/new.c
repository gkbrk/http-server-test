#include "buffer.h"

Buffer buffer_new() {
  Buffer b;
  b.size = 0;
  b.capacity = 0;
  b.data = NULL;
  return b;
}
