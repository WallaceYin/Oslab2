#include <os.h>
#include <debug.h>

static void os_init();
static void os_run();
static _RegSet *os_interrupt(_Event ev, _RegSet *regs);
static void idel_run();

MOD_DEF(os) {
  .init = os_init,
  .run = os_run,
  .interrupt = os_interrupt,
};
static _RegSet *idel_regset;
static void os_init() {
	_Area kstack;
	kstack.start = pmm->alloc(REGSET_SIZE);
	kstack.end = kstack.start + REGSET_SIZE;
	idel_regset = _made(kstack, idel_run, NULL);
	printf("Hello, OS World!\n");
}

static void test_run();
static void os_run() {
  _intr_write(1); // enable interrupt
#ifdef DEBUG
	test_run();
#endif
  while (1) ; // should never return
}

static _RegSet *os_interrupt(_Event ev, _RegSet *regs) {
	if (current_thread != NULL)
		current_thread->regset = regs;
	thread_t *p = kmt->schedule();
	if (p == NULL)
		return idel_regset;
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
			//TODO: irq_iodev
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

static void f(void *arg) {
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
	
}

static void idel_run(void *arg) {
	while (1);
}
