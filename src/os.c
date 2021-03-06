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

char s1[100];
static void print_1(void *arg) {
	int fd = vfs->open("/proc/cpuinfo", O_RDONLY);
	memset(s1, 0, 100);
	vfs->read(fd, (void *)s1, 100);
	for (int i = 0; i < strlen(s1); i++)
		_putc(s1[i]);
	for (;;) {}
}

char s2[100];
static void print_2(void *arg) {
	int fd = vfs->open("/proc/meminfo", O_RDONLY);
	memset(s2, 0, 100);
	vfs->read(fd, (void *)s2, 100);
	for (int i = 0; i < strlen(s2); i++)
		_putc(s2[i]);
	for (;;) {}
}

char s3[100];
static void print_3(void *arg) {
	int fd = vfs->open("/proc/0", O_RDONLY);
	memset(s3, 0, 100);
	vfs->read(fd, (void *)s3, 100);
	for (int i = 0; i < strlen(s3); i++)
		_putc(s3[i]);
	for (;;) {}
}

thread_t t1, t2, t3;
static void test_run() {
	kmt->create(&t1, print_1, NULL);
	kmt->create(&t2, print_2, NULL);
	kmt->create(&t3, print_3, NULL);
}
#endif
