#ifndef __OS_H__
#define __OS_H__

#include <kernel.h>
#define THREAD_SIZE 12
#define REGSET_SIZE 64
static inline void puts(const char *p) {
  for (; *p; p++) {
    _putc(*p);
  }
}

struct thread {
	_RegSet *regset;
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
