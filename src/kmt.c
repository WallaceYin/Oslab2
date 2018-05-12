#include <os.h>

static void kmt_init();
static int kmt_create(thread_t *thread, void (*entry)(void *arg), void *arg);
static void kmt_teardown(thread_t *thread);
static thread_t *kmt_schedule();
static void spin_init(spinlock_t *lk, const char *name);
static void spin_lock(spinlock_t *lk);
static void spin_unlock(spinlock_t *lk);
static void sem_init(sem_t *sem, const char *name, int value);
static void sem_wait(sem_t *sem);
static void sem_signal(sem_t *sem);

MOD_DEF(kmt) {
	.init = kmt_init,
	.create = kmt_create,
	.teardown = kmt_teardown,
	.spin_init = spin_init,
	.spin_lock = spin_lock,
	.spin_unlock = spin_unlock,
	.sem_init = sem_init,
	.sem_wait = sem_wait,
	.sem_signal = sem_signal,
};

static void kmt_init() {
	//TODO
}

static int kmt_create(thread_t *thread, void (*entry)(void *arg), void *arg) {
	
	//TODO
}

static void kmt_teardown(thread_t *thread) {
	//TODO
}

static thread_t *kmt_schedule() {
	//TODO
}

static void spin_init(spinlock_t *lk, const char *name) {
	//TODO
}

static void spin_lock(spinlock_t *lk) {
	//TODO
}

static void spin_unlock(spinlock_t *lk) {
	//TODO
}

static void sem_init(sem_t *sem, const char *name, int value) {
	//TODO
}

static void sem_wait(sem_t *sem) {
	//TODO
}

static void sem_signal(sem_t *sem) {
	//TODO
}
