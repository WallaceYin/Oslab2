#ifndef __OS_H__
#define __OS_H__

#include <kernel.h>
#define THREAD_SIZE 20
#define REGSET_SIZE 64
#define KSTACK_SIZE 4096
#define MAX_THREAD 32

static inline void puts(const char *p) {
  for (; *p; p++) {
    _putc(*p);
  }
}

struct thread {
	_RegSet *regset;
	void *kstack;
	int pid;
	int free; //free = the process is running ? 0 : 1;
};
typedef	struct thread thread_t;
thread_t tlist[MAX_THREAD];
// WARNING: It is possible that if you define DEBUG in debug.h, you process stack overflowed, due to the fact that Log would cost much of your stack area.

struct spinlock {
	const char *nam;
	int locked;
};
typedef struct spinlock spinlock_t;

struct semaphore {
	int count;
	const char *nam;
	int sleep_id;
};
typedef struct semaphore sem_t;

#endif
