#ifndef LEO_HTTP_REQUEST_H_
#define LEO_HTTP_REQUEST_H_

#include <stdbool.h>

#include "buffer/buffer.h"

typedef struct {
  bool filled;
  Span name;
  Span value;
} RequestHeader;

typedef struct {
  Span method;
  Span url;
  uint8_t version;
  RequestHeader headers[16];
  Span body;
} Request;

bool http_request_parse(Buffer *buf, Request *req);

#endif
