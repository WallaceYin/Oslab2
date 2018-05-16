#include <os.h>
#include <debug.h>

static void os_init();
static void os_run();
static _RegSet *os_interrupt(_Event ev, _RegSet *regs);
static int last_id;

MOD_DEF(os) {
  .init = os_init,
  .run = os_run,
  .interrupt = os_interrupt,
};

static void os_init() {
	last_id = -1;
	printf("Hello, OS World!\n");
}

static void test_run();
static void os_run() {
#ifdef DEBUG
	test_run();
#endif
	_intr_write(1); // enable interrupt
  while (1) ; // should never return
}

static _RegSet *os_interrupt(_Event ev, _RegSet *regs) {
	if (last_id != -1)
		tlist[last_id].regset = regs;
	thread_t *p = kmt->schedule();
	last_id = p->pid;
	switch (ev.event) {
		case _EVENT_IRQ_TIMER:
#ifdef DEBUG
			Log("Irq_timer called.");
#endif
			break;

		case _EVENT_IRQ_IODEV:
#ifdef DEBUG
			Log("Irq_iodev called.");
#endif
			break;

		case _EVENT_ERROR:
#ifdef DEBUG
			Log("An Error happeded.");
#endif
			_halt(1);
			break;
	}

	return p->regset;
}

/*static void f(void *arg) {
	while (1) {
		_putc(*(char *)arg);
	}
}

static void test_run(void) {
	thread_t t1, t2;
	char c1,c2;
	c1 = 'a';
	c2 = 'b';
	kmt->create(&t1, f, (void*)&c1);
	Log("Continue");
	kmt->create(&t2, f, (void*)&c2);
	
}*/

sem_t empty, fill;
static void producer(void *arg) {
	while (1) {
		kmt->sem_wait(&empty);
		_putc('[');
		kmt->sem_signal(&fill);
	}
}

static void consumer(void *arg) {
	while (1) {
		kmt->sem_wait(&fill);
		_putc(']');
		kmt->sem_signal(&empty);
	}
}

thread_t t1, t2;
static void test_run() {
	kmt->sem_init(&empty, "empty", 3);
	kmt->sem_init(&fill, "fill", 0);
	kmt->create(t1, producer, NULL);
	kmt->create(t2, producer, NULL);
}
