#include <stddef.h>

#include "minitask.h"

static void minitask_swap(MiniTask *task1, MiniTask *task2) {
  MiniTask temp;
  temp.active = task2->active;
  temp.data   = task2->data;
  temp.iter   = task2->iter;
  temp.init   = task2->init;
  temp.step   = task2->step;
  temp.end    = task2->end;

  task2->active = task1->active;
  task2->data   = task1->data;
  task2->iter   = task1->iter;
  task2->init   = task1->init;
  task2->step   = task1->step;
  task2->end    = task1->end;

  task1->active = temp.active;
  task1->data   = temp.data;
  task1->iter   = temp.iter;
  task1->init   = temp.init;
  task1->step   = temp.step;
  task1->end    = temp.end;
}

static void minitask_dummy(MiniTask *task) {
  (void)task;
}

MiniTaskRunner minitask_init_static(MiniTask *tasks, size_t capacity) {
  MiniTaskRunner runner;
  runner.tasks = tasks;
  runner.capacity = capacity;
  runner.currentTask = 0;
  return runner;
}

MiniTask *minitask_get_free(MiniTaskRunner *runner) {
  MiniTask *task = NULL;
  size_t i;
  for (i = 0; i < runner->capacity; i++) {
    if (!runner->tasks[i].active) {
      task = &runner->tasks[i];
      task->active = true;
      task->iter = 0;
      task->init = minitask_dummy;
      task->step = minitask_dummy;
      task->end = minitask_dummy;
      break;
    }
  }
  return task;
}

bool minitask_step(MiniTaskRunner *runner) {
  MiniTask *task = &runner->tasks[runner->currentTask];
  MiniTask *last_active = NULL;
  size_t i = 0;

  if (!task->active) {
    runner->currentTask = 0;
    return 1;
  }

  if (task->init != minitask_dummy) {
    task->init(task);
    task->init = minitask_dummy;
  }

  task->step(task);
  task->iter++;
  runner->currentTask++;
  runner->currentTask %= runner->capacity;

  if (!task->active) {
    task->end(task);

    /* Defrag */
    for (i = runner->capacity; i != 0; i--) {
      if (&runner->tasks[i - 1] == task) {
        last_active = task;
        break;
      }

      if (runner->tasks[i - 1].active) {
        last_active = &runner->tasks[i - 1];
        break;
      }
    }

    if (last_active == NULL) {
      return false;
    } else if (last_active != task) {
      minitask_swap(task, last_active);
    }
  }

  return true;
}

void minitask_spawn(MiniTaskRunner *runner, void (*step)(MiniTask *task)) {
  MiniTask *task = minitask_get_free(runner);
  task->step = step;
}
