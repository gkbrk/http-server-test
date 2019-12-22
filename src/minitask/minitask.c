#include <stddef.h>
#include <string.h>

#include "minitask.h"

static void minitask_swap(MiniTask *task1, MiniTask *task2) {
  MiniTask temp;
  memcpy(&temp, task2, sizeof(MiniTask));
  memcpy(task2, task1, sizeof(MiniTask));
  memcpy(task1, &temp, sizeof(MiniTask));
}

static void minitask_dummy(MiniTask *task) {
  (void)task;
}

MiniTaskRunner minitask_init_static(MiniTask *tasks, size_t capacity) {
  MiniTaskRunner runner;
  runner.tasks = tasks;
  runner.capacity = capacity;
  runner.currentTask = 0;
  runner.size = 0;
  return runner;
}

MiniTask *minitask_get_free(MiniTaskRunner *runner) {
  MiniTask *task = NULL;

  if (runner->size < runner->capacity) {
    task = &runner->tasks[runner->size];
    task->active = true;
    task->iter = 0;
    task->init = minitask_dummy;
    task->step = minitask_dummy;
    task->end = minitask_dummy;

    runner->size++;
  }

  return task;
}

bool minitask_step(MiniTaskRunner *runner) {
  MiniTask *task = &runner->tasks[runner->currentTask];

  if (task->init != minitask_dummy) {
    task->init(task);
    task->init = minitask_dummy;
  }

  task->step(task);
  task->iter++;

  if (!task->active) {
    size_t size = runner->size;
    task->end(task);
    runner->size--;

    if (size == 1) {
      return false;
    } else if (runner->currentTask != size - 1) {
      minitask_swap(task, &runner->tasks[size - 1]);
    }
  }

  runner->currentTask++;
  runner->currentTask %= runner->size;

  return true;
}

void minitask_spawn(MiniTaskRunner *runner, void (*step)(MiniTask *task)) {
  MiniTask *task = minitask_get_free(runner);
  task->step = step;
}
