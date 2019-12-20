#include "request.h"
#include <stdio.h>

#define bool_assert(x) if (!(x)) return false;

static inline char get_char(Buffer *buf, char **ptr) {
  if ((size_t)(*ptr - buf->data) < buf->size) {
    *ptr = *ptr + 1;
    return *(*ptr - 1);
  }

  return 0;
}

bool http_request_parse(Buffer *buf, Request *req) {
  char *ptr = buf->data;

  req->method.from = ptr;
  req->method.size = 0;
  while (1) {
    char c = get_char(buf, &ptr);
    req->method.size++;
    bool_assert(c);
    if (c == ' ') break;
  }

  req->url.from = ptr;
  req->url.size = 0;
  while (1) {
    char c = get_char(buf, &ptr);
    req->url.size++;
    bool_assert(c);
    if (c == ' ') break;
  }

  bool_assert(get_char(buf, &ptr) == 'H');
  bool_assert(get_char(buf, &ptr) == 'T');
  bool_assert(get_char(buf, &ptr) == 'T');
  bool_assert(get_char(buf, &ptr) == 'P');
  bool_assert(get_char(buf, &ptr) == '/');
  bool_assert(get_char(buf, &ptr) == '1');
  bool_assert(get_char(buf, &ptr) == '.');

  char version = get_char(buf, &ptr);
  bool_assert(version);
  req->version = version - '0';

  bool_assert(get_char(buf, &ptr) == '\r');
  bool_assert(get_char(buf, &ptr) == '\n');

  return true;
}
