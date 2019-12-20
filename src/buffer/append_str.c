#include <string.h>

#include "buffer.h"

void buffer_append_str(Buffer *buf, char *data) {
  buffer_append(buf, data, strlen(data));
}
