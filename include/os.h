#ifndef __OS_H__
#define __OS_H__

#include <kernel.h>
#include <debug.h>
#define THREAD_SIZE 16
static inline void puts(const char *p) {
  for (; *p; p++) {
    _putc(*p);
  }
}

struct thread {
	void (*entry)(void *arg);
	void *arg;
	struct thread *next; // next thread
	int free; //free = the process is running ? 0 : 1;
};
typedef	struct thread thread_t;

struct spinlock {
	const char *nam;
	int locked;
};
typedef struct spinlock spinlock_t;

struct semaphore {
	//TODO
};
typedef struct semaphore sem_t;

#endif
