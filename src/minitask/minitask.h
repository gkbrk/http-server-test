#ifndef LEO_MINITASK_H_
#define LEO_MINITASK_H_

#include <stdbool.h>
#include <stddef.h>

typedef struct MiniTask MiniTask;
typedef void (miniTaskCallback)(MiniTask *task);

struct MiniTask {
  bool active;
  void *data;
  size_t iter;
  miniTaskCallback *init;
  miniTaskCallback *step;
  miniTaskCallback *end;
};

typedef struct {
  MiniTask *tasks;
  size_t currentTask;
  size_t size;
  size_t capacity;
} MiniTaskRunner;

MiniTask *minitask_get_free(MiniTaskRunner *runner);
void minitask_spawn(MiniTaskRunner *runner, miniTaskCallback *step);
bool minitask_step(MiniTaskRunner *runner);
MiniTaskRunner minitask_init_static(MiniTask *tasks, size_t capacity);

#endif
