#ifndef LEO_MINITASK_H
#define LEO_MINITASK_H

#include <stdbool.h>
#include <stddef.h>

#define MiniTask struct structMiniTask
#define MiniTaskRunner struct structMiniTaskRunner

struct structMiniTask {
  bool active;
  void *data;
  size_t iter;
  void (*init)(MiniTask *task);
  void (*step)(MiniTask *task);
  void (*end)(MiniTask *task);
};

struct structMiniTaskRunner {
  MiniTask *tasks;
  size_t currentTask;
  size_t capacity;
};

MiniTask *minitask_get_free(MiniTaskRunner *runner);
void minitask_spawn(MiniTaskRunner *runner, void (*step)(MiniTask *task));
bool minitask_step(MiniTaskRunner *runner);
MiniTaskRunner minitask_init_static(MiniTask *tasks, size_t capacity);

#endif
