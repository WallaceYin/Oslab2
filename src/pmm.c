#include <os.h>
#include <debug.h>
#define BLOCK_SIZE 16
static void pmm_init();
static void *pmm_alloc(size_t size);
static void pmm_free(void *ptr);

static void *current;

struct _Block {
	size_t size; // Aligned size
	void *start; // the start of block area
	int free; // free = this block is occupied? 0 : 1;
	void *next; // the next block
}; 

struct _Block *block;

MOD_DEF(pmm) {
	.init = pmm_init,
	.alloc = pmm_alloc,
	.free = pmm_free,
};

static void pmm_init() {
	current = _heap.start;
	if (current == NULL)
		perror("Danger!");
	Log("Inited\n");
	block = NULL;
}

static void *pmm_alloc(size_t size) {
	int digit(size_t size) {
		int d = 0;
		while (size != 0)
		{
			size = size >> 1;
			d += 1;
		}
		return d;
	};

	size_t align(int digit) {
		return 1 << (digit - 1);
	};

	if (size == 0)
	{
		return NULL;
	}
	Log("New block to be created with size %d", size);
	if (block == NULL)
	{
		void *p = current;
		while (((size_t)p) % align(digit(size + BLOCK_SIZE)) != 0) p++;
		current = p + align(digit(size + BLOCK_SIZE));
		//Log("current = %8x and p = %8x", (unsigned)current, (unsigned)p);
		block = (struct _Block *)p + size;
		block->size = align(digit(size + BLOCK_SIZE)) - BLOCK_SIZE;
		block->start = p;
		block->free = 0;
		block->next = NULL;
		if (p == NULL)
			perror("Error here");
		return p;
	}
	else
	{
		struct _Block *p = block;
		while (p->next != NULL)
		{
			if (p->size >= size && p->free)
				return p->start;
			p = p->next;
		}
		if (p->size >= size && p->free)
			return p->start;
		void *q = current;
		while (((size_t)q) % align(digit(size + BLOCK_SIZE)) != 0) q++;
		current = q + align(digit(size + BLOCK_SIZE));
		block = (struct _Block *)p + size;
		p->size = align(digit(size + BLOCK_SIZE)) - BLOCK_SIZE;
		p->start = q;
		p->free = 0;
		p->next = NULL;
		return p->start;
	}
}

static void pmm_free(void *ptr) {
	struct _Block *p = block;
	while (p->next != NULL)
	{
		if (p->start == ptr)
		{
			p->free = 1;
			break;
		}
		p = p->next;
	}
	if (p->start == ptr)
		p->free = 1;
}
