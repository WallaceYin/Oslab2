#ifndef __OS_H__
#define __OS_H__

#include <kernel.h>
#define THREAD_SIZE 20
#define REGSET_SIZE 64
#define KSTACK_SIZE 4096
#define PIECE_SIZE 4096
#define MAX_THREAD 32
#define MAX_BLOCK 4
#define MAX_FILE 64
#define MAX_FD 16
#define PATH_LEN 20

static inline void puts(const char *p) {
  for (; *p; p++) {
    _putc(*p);
  }
}

struct inode {
	int id;
	size_t size;
	int flags;
	int num_block;
	void *block[MAX_BLOCK];
};
typedef struct inode inode_t;


struct thread {
	_RegSet *regset;
	void *kstack;
	int pid;
	int free; //free = the process is running ? 0 : 1;
	file_t fdlist[MAX_FD];
};
typedef	struct thread thread_t;
thread_t tlist[MAX_THREAD];
int last_id;
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

struct filemap {
	char path[PATH_LEN];
	inode_t inode;
};
typedef struct filemap filemap_t;

struct filesystem {
	char root[10];
	int num_file;
	filemap_t Filemap[MAX_FILE];
};
typedef struct filesystem filesystem_t;
filesystem_t *procfs;
filesystem_t *devfs;
filesystem_t *kvfs;

#define O_RDONLY 1
#define O_WRONLY 2
#define O_RDWR 3

/*struct inode {
	int id; // inode_id
	size_t size; // file size
	int flags; // RO WO or WR
	int num_block; // number of blocks
	void *block[MAX_BLOCK]; // nodes for blocks
};
typedef struct inode inode_t;*/

struct file {
	int fd; // file descripter
	filesystem_t *mount;
	off_t offset; // offset
	inode_t *inode; // inode of file
};
typedef struct file file_t;

#endif
