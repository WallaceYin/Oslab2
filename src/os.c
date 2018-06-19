#include <os.h>
#include <debug.h>
#include <ylib.h>
#define TEST

static void os_init();
static void os_run();
static _RegSet *os_interrupt(_Event ev, _RegSet *regs);
//static int last_id;

MOD_DEF(os) {
  .init = os_init,
  .run = os_run,
  .interrupt = os_interrupt,
};

static void os_init() {
	last_id = -1;
	vfs->init();
	printf("Hello, OS World!\n");
}
#ifdef TEST
static void test_run();
#endif
static void os_run() {
#ifdef TEST
	test_run();
#endif
	_intr_write(1); // enable interrupt
  while (1) ; // should never return
}

static _RegSet *os_interrupt(_Event ev, _RegSet *regs) {
	switch (ev.event) {
		case _EVENT_IRQ_TIMER:
#ifdef DEBUG
			Log("Irq_timer called.");
#endif
			if (last_id != -1)
				memcpy(tlist[last_id].regset, (const void *)regs, REGSET_SIZE);
			thread_t *p = kmt->schedule();
			last_id = p->pid;
			return p->regset;
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

	return NULL;
}

#ifdef TEST

char s[30];
static void printinfo(void *arg) {
	int fd = vfs->open("/proc/cpuinfo", O_RDONLY);
	memset(s, 0, 30);
	vfs->read(fd, (void *)s, 30);
	for (int i = 0; i < strlen(s); i++)
		_putc(s[i]);
}

thread_t t1;
static void test_run() {
	kmt->create(&t1, printinfo, NULL);
}
#endif
