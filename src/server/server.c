#define _POSIX_C_SOURCE  200809L
#include <errno.h>
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

#define PORT 6969

static MiniTask tasks[256];
static MiniTaskRunner runner;

typedef struct {
  int accept_fd;
} AcceptData;

typedef struct {
  int fd;
  Buffer buffer;
} ConnectionData;

static void ConnectionTask(MiniTask *task);
static void ConnectionClean(MiniTask *task);

static void AcceptInit(MiniTask *task) {
  AcceptData *data = malloc(sizeof(AcceptData));
  data->accept_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (data->accept_fd < 0) {
    fprintf(stderr, "Could not create socket");
    exit(1);
  }

  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  int optval = 1;
  setsockopt(data->accept_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

  ssize_t err = bind(data->accept_fd, (struct sockaddr*) &server_addr, sizeof(server_addr));
  if (err != 0) {
    fprintf(stderr, "Could not bind socket");
    exit(1);
  }

  listen(data->accept_fd, 69);
  fcntl(data->accept_fd, F_SETFL, O_NONBLOCK);

  task->data = data;
}

static void AcceptTask(MiniTask *task) {
  AcceptData *data = task->data;
  struct sockaddr_in client_addr;
  socklen_t client_addrlen = 0;
  int client_fd = accept(data->accept_fd, (struct sockaddr*)&client_addr, &client_addrlen);
  if (client_fd < 0) return;
  // printf("Accepted connection\n");

  MiniTask *connTask = minitask_get_free(&runner);
  if (connTask != NULL) {
    connTask->step = ConnectionTask;
    connTask->end = ConnectionClean;

    ConnectionData *connData = malloc(sizeof(ConnectionData));
    connData->fd = client_fd;
    connData->buffer = buffer_with_capacity(4096);
    connTask->data = connData;
  }
}

#define STATIC_WRITE(x) write(data->fd, (x), (sizeof(x)));

static void ConnectionTask(MiniTask *task) {
  ConnectionData *data = task->data;
  Request req;
  if (http_request_parse(&data->buffer, &req)) {
    if (strncmp(req.url.from, "/exit ", req.url.size) == 0) exit(0);
    STATIC_WRITE("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\nPHP is the best\n");
    /*
    dprintf(data->fd, "HTTP/1.1 200 OK\r\n");
    dprintf(data->fd, "Content-Type: text/html\r\n");
    dprintf(data->fd, "\r\n");
    dprintf(data->fd, "<h3>PHP is the best</h3>");
    dprintf(data->fd, "<ul>");
    dprintf(data->fd, "<li><b>HTTP Method:</b> %.*s</li>", (int)req.method.size, req.method.from);
    dprintf(data->fd, "<li><b>HTTP Version:</b> %d</li>", req.version);
    dprintf(data->fd, "<li><b>URL:</b> %.*s</li>", (int)req.url.size, req.url.from);
    dprintf(data->fd, "</ul>");
    */
    close(data->fd);
    task->active = false;
  }
  //char buffer[4096];
  ssize_t len = recv(data->fd, data->buffer.data + data->buffer.size, 4096, 0);
  if (len > 0) {
    data->buffer.size += len;
    // buffer_append(&data->buffer, buffer, len);
    return;
  }
  if (len == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) return;
  if (len == -1) task->active = false;
}

static void ConnectionClean(MiniTask *task) {
  ConnectionData *data = task->data;
  free(data->buffer.data);
  free(data);
}

void server_run(void) {
  runner = minitask_init_static(tasks, 50);

  MiniTask *task = minitask_get_free(&runner);
  task->init = AcceptInit;
  task->step = AcceptTask;

  while (minitask_step(&runner));
}
