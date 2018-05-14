#include <os.h>
#include <debug.h>
static void os_init();
static void os_run();
static _RegSet *os_interrupt(_Event ev, _RegSet *regs);

MOD_DEF(os) {
  .init = os_init,
  .run = os_run,
  .interrupt = os_interrupt,
};

static void os_init() {
  /*for (const char *p = "Hello, OS World!\n"; *p; p++) {
    _putc(*p);
  }*/
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
	switch (ev.event) {
		case _EVENT_IRQ_TIMER:
#ifdef DEBUG
			Log("Irq_timer called.\n");
			Log("Timer status: %d\n", _intr_read());
#endif
			
			break;

		case _EVENT_IRQ_IODEV:
#ifdef DEBUG
			Log("Irq_iodev called.\n");
#endif
			//TODO: irq_iodev
			break;

		case _EVENT_ERROR:
#ifdef DEBUG
			Log("An Error happeded.\n");
#endif
			_halt(1);
			break;
	}

  /*if (ev.event == _EVENT_IRQ_TIMER) _putc('*');
  if (ev.event == _EVENT_IRQ_IODEV) _putc('I');
  if (ev.event == _EVENT_ERROR) {
    _putc('x');
    _halt(1);
  }*/
  return NULL; // this is allowed by AM
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
	kmt->create(&t2, f, (void*)&c2);
	
}
