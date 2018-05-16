#include <os.h>
#include <debug.h>

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
static int current_id;
static int pid_num;
static int tlist_len;

MOD_DEF(kmt) {
	.init = kmt_init,
	.create = kmt_create,
	.teardown = kmt_teardown,
	.schedule = kmt_schedule,
	.spin_init = spin_init,
	.spin_lock = spin_lock,
	.spin_unlock = spin_unlock,
	.sem_init = sem_init,
	.sem_wait = sem_wait,
	.sem_signal = sem_signal,
};

static void kmt_init() {
	tlist_len = 0;
	pid_num = 0;
	current_id = -1;
}

static int kmt_create(thread_t *thread, void (*entry)(void *arg), void *arg) {
#ifdef DEBUG
	Log("New thread created, arg is %c", *(char *)arg);
#endif
	int nthread = -1;
	for (int i = 0; i < tlist_len; i++)
		if (tlist[i].free == 1)
		{
			nthread = i;
			break;
		}
	if (nthread == -1)
		nthread = tlist_len;
	tlist_len = tlist_len + 1;
	if (tlist_len > MAX_THREAD)
	{
		perror("Thread list overfull.");
		_halt(1);
	}
	tlist[nthread].free = 0;
	thread->free = 0;
	tlist[nthread].kstack = pmm->alloc(KSTACK_SIZE);
	_Area Kstack;
	Kstack.start = tlist[nthread].kstack;
	Kstack.end = Kstack.start + MAX_THREAD;
	thread->kstack = Kstack.start;
	tlist[nthread].regset = _make(Kstack, entry, arg);
	thread->regset = tlist[nthread].regset;
	tlist[nthread].pid = pid_num;
	thread->pid = tlist[nthread].pid;
	pid_num++;
	current_id = nthread;
	if (_intr_read())
		((void(*)(void *))entry)(arg);
	return tlist[nthread].pid;
}

static void kmt_teardown(thread_t *thread) {	
	int found = 0;
	for (int i = 0; i < tlist_len; i++)
		if (tlist[i].pid == thread->pid)
		{
			tlist[i].free = 1;
			found = 1;
			if (current_id == i)
			{
				for (int j = 0; j < tlist_len; j++)
					if (!tlist[j].free)
						current_id = j;
			}
		}
	if (!found)
	{
		perror("This thread is not included in thread list!\n");
	}
}

static thread_t *kmt_schedule() {
	Log("kmt_schedule triggered.");
	thread_t *p;
	p = &tlist[current_id];
	int next_id = -1;
	for (int i = current_id + 1; i < tlist_len; i++)
		if (tlist[i].free == 0)
		{
			next_id = i;
			current_id = next_id;
			return p;
		}
	if (next_id == -1)
		for (int i = 0; i < current_id; i++)
			if (tlist[i].free == 0)
			{
				next_id = i;
				current_id = next_id;
				return p;
			}
	if (next_id == -1)
		return p;
	if (current_id == -1)
	{
		perror("Serious problem happened.");
		_halt(1);
		return NULL;
	}
	
	perror("Warning! Should not reach here.");
	while (1);
	return NULL;
}

static void spin_init(spinlock_t *lk, const char *name) {
	lk->nam = name;
	lk->locked = 0;
	
}

static void spin_lock(spinlock_t *lk) {
#ifdef DEBUG
	Log("Locked: %s", lk->nam);
#endif
	lk->locked = 1;
	_intr_write(0);
}

static void spin_unlock(spinlock_t *lk) {
#ifdef DEBUG
	Log("Unlocked: %s", lk->nam);
#endif
	lk->locked = 0;
	_intr_write(1);
}

static void sem_init(sem_t *sem, const char *name, int value) {
	sem->count = value;
	sem->nam = name;
	sem->sleep_id = -1;
}

static void sem_wait(sem_t *sem) {
	sem->count--;
	if (sem->count < 0)
	{
		if (current_id == -1)
		{
			perror("Error! No process is running.");
			_halt(1);
		}
		sem->sleep_id = current_id;
		tlist[current_id].free = 1;
		_yield();
	}
}

static void sem_signal(sem_t *sem) {
	sem->count++;
	if (sem->sleep_id > -1)
	{
		tlist[sem->sleep_id].free = 0;
		current_id = sem->sleep_id;
		sem->sleep_id = -1;
		_yield();
	}
}
