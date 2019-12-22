#define _POSIX_C_SOURCE  200809L
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "server.h"
#include "buffer/buffer.h"
#include "http/request.h"
#include "recycler/recycler.h"

#define PORT 6969

static MiniTask tasks[256];
static MiniTaskRunner runner;

typedef struct {
  int fd;
  Buffer buffer;
} ConnectionData;

static void ConnectionTask(MiniTask *task);
static void ConnectionClean(MiniTask *task);

static int accept_fd;

static Recycler connRecycler;

static void AcceptInit(void) {
  accept_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (accept_fd < 0) {
    fprintf(stderr, "Could not create socket");
    exit(1);
  }

  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  int optval = 1;
  setsockopt(accept_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

  ssize_t err = bind(accept_fd, (struct sockaddr*) &server_addr, sizeof(server_addr));
  if (err != 0) {
    fprintf(stderr, "Could not bind socket");
    exit(1);
  }

  listen(accept_fd, 69);
  fcntl(accept_fd, F_SETFL, O_NONBLOCK);
}

static void AcceptTask(MiniTask *task) {
  (void)task;
  struct sockaddr_in client_addr;
  socklen_t client_addrlen = 0;
  for (;;) {
    if (runner.size > 1) {
      fcntl(accept_fd, F_SETFL, O_NONBLOCK);
    } else {
      fcntl(accept_fd, F_SETFL, 0);
    }
    int client_fd = accept(accept_fd, (struct sockaddr*)&client_addr, &client_addrlen);
    if (client_fd < 0) return;
    fcntl(client_fd, F_SETFL, O_NONBLOCK);

    MiniTask *connTask = minitask_get_free(&runner);
    if (connTask != NULL) {
      connTask->step = ConnectionTask;
      connTask->end = ConnectionClean;

      ConnectionData *connData = recycler_get(&connRecycler);
      connData->fd = client_fd;
      buffer_clear(&connData->buffer);
      connTask->data = connData;
    }
  }
}

#define STATIC_WRITE(x) write(data->fd, (x), sizeof((x)) - 1)
#define WRITE_SPAN(x) write(data->fd, x.from, x.size)

static void ConnectionTask(MiniTask *task) {
  ConnectionData *data = task->data;
  Request req;
  if (http_request_parse(&data->buffer, &req)) {
    STATIC_WRITE("HTTP/1.1 200 OK\r\n"
                 "Content-Type: text/html\r\n"
                 "Content-Length: 25\r\n"
                 "\r\n"
                 "<h3>PHP is the best</h3>\n");
    close(data->fd);
    task->active = false;
    return;
  }
  // char buffer[4096];
  ssize_t len = recv(data->fd, data->buffer.data + data->buffer.size, 4096, 0);
  if (len > 0) {
    // buffer_append(&data->buffer, buffer, len);
    data->buffer.size += len;
    return;
  }
  if (len == 0) task->active = false;
  if (len == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) return;
  if (len == -1) {
    close(data->fd);
    task->active = false;
  }
}

static void ConnectionClean(MiniTask *task) {
  ConnectionData *data = task->data;
  recycler_recycle(&connRecycler, data);
}

void *newconn() {
  ConnectionData *data = malloc(sizeof(ConnectionData));
  data->buffer = buffer_with_capacity(4096);
  return data;
}

void server_run(void) {
  signal(SIGPIPE, SIG_IGN);
  AcceptInit();
  runner = minitask_init_static(tasks, 255);

  connRecycler = recycler_new(newconn, NULL);

  MiniTask *task = minitask_get_free(&runner);
  task->step = AcceptTask;

  while (minitask_step(&runner));
}
