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

static thread_t *kmt_head;
static thread_t *current_thread;
static void kmt_init() {
	kmt_head = NULL;
	current_thread = NULL;
}

static int kmt_create(thread_t *thread, void (*entry)(void *arg), void *arg) {
	if (kmt_head == NULL)
	{
		kmt_head = pmm->alloc(THREAD_SIZE);
		if (kmt_head == NULL)
		{
			perror("Error happend when pmm_alloc");
			_halt(1);
		}
		kmt_head->entry = entry;
		kmt_head->arg = arg;
		kmt_head->next = NULL;
		kmt_head->free = 1;
		if (/* */ 0)
		{
			kmt_head->free = 0;
			current_thread = kmt_head;
			((void(*)(void *))kmt_head->entry)(kmt_head->arg);
		}
	}
	else
	{
		thread_t *p;
		p = kmt_head;
		while (p->next != NULL)
			p = p->next;
		p->next = pmm->alloc(THREAD_SIZE);
		p = p->next;
		p->entry = entry;
		p->arg = arg;
		p->next = NULL;
		p->free = 1;
		if (/* */ 0)
		{
			p->free = 0;
			current_thread = p;
			((void(*)(void *))p->entry)(p->arg);
		}
	}
	return 0;
}

static void kmt_teardown(thread_t *thread) {
	if (!thread->free)
	{
		perror("Warning! You are going to free a thread in use!");
		_halt(1);
	}
	thread_t *p;
	p = kmt_head;
	while (p->next != thread && p->next != NULL)
		p = p->next;
	if (p->next != thread)
	{
		perror("Oooops! this thread is not contained in thread list.");
		_halt(1);
	}
	p->next = p->next->next;
	pmm->free(thread);
}

static thread_t *kmt_schedule() {
	if (current_thread == NULL)
		return NULL;
	if (current_thread->free == 0)
	{
		perror("Error! some serious problem triggered.");
		_halt(1);
	}
	current_thread->free = 1;
	if (current_thread->next != NULL)
	{
		current_thread->next->free = 0;
		current_thread = current_thread->next;
		return current_thread;
	}
	else if (current_thread->next == kmt_head)
	{
		perror("Warning! There are no avaliable thread now.");
		_halt(1);
	}
	else
	{
		kmt_head->free = 0;
		current_thread = kmt_head;
		return current_thread;
	}
	
	perror("Warning! Should not reach here.");
	while (1);
	return NULL;
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