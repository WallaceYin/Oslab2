#include <os.h>
#include <debug.h>

static void vfs_init();
static int vfs_access(const char *path, int mode);
static int vfs_mount(const char *path, filesystem_t *fs);
static int vfs_unmount(const char *path);
static int vfs_open(const char *path, int flags);
static void vfs_create(filesystem_t *fs, char *path, int flags);
static ssize_t vfs_read(int fd, void *buf, size_t nbyte);
static ssize_t vfs_write(int fd, void *buf, size_t nbyte);
static off_t vfs_lseek(int fd, off_t offset, int whence);
static int vfs_close(int fd);

MOD_DEF(vfs) {
	.init = vfs_init,
	.access = vfs_access,
	.mount = vfs_mount,
	.unmount = vfs_unmount,
	.open = vfs_open,
	.create = vfs_create,
	.read = vfs_read,
	.write = vfs_write,
	.lseek = vfs_lseek,
	.close = vfs_close,
};

static void vfs_init() {
	procfs = (filesystem_t *)pmm->alloc(sizeof(filesystem_t));
	devfs = (filesystem_t *)pmm->alloc(sizeof(filesystem_t));
	kvfs = (filesystem_t *)pmm->alloc(sizeof(filesystem_t));
	vfs->mount("/proc", procfs);
	vfs->mount("/dev", devfs);
	vfs->mount("/", kvfs);
	
	vfs->create(procfs, "/cpuinfo", O_RDONLY);
	vfs->create(procfs, "/meminfo", O_RDONLY);
	vfs->create(devfs, "/null", O_RDWR);
	vfs->create(devfs, "/zero", O_RDWR);
	vfs->create(devfs, "/random", O_RDWR);
	//TODO
}

static int vfs_access(const char *path, int mode) {
	//TODO
}

static int vfs_mount(const char *path, filesystem_t *fs) {
	strcpy(fs->root, path);
	fs->num_file = 0;
	return 0;
}

static int vfs_unmount(const char *path) {
	return -1;
}

static int vfs_open(const char *path, int flags) {
	filesystem_t *fs = NULL;
	if (strstr(path, "/proc") == 0)
		fs = procfs;
	else if (strstr(path, "/dev") == 0)
		fs = devfs;
	else if (strstr(path, "/") == 0)
		fs = kvfs;
	if (fs == NULL)
		return -1;
	if (last_id == -1)
		perror("Error happened in process schedule.");
	for (int i = 0; i < fs->num_file; i++)
		if (strcmp(fs->Filemap[i].path, path) == 0)
		{
			int j = 0;
			while (tlist[last_id].fdlist[j].fd != -1)
				i++;
			tlist[last_id].fdlist[j].fd = j;
			tlist[last_id].fdlist[j].mount = fs;
			tlist[last_id].fdlist[j].offset = 0;
			tlist[last_id].fdlist[j].inode = &fs->Filemap[i].inode;
			return j;
		}
	return -1;
}

static void vfs_create(filesystem_t *fs, char *path, int flags) {
	strcpy(fs->Filemap[num_file].path, path);
	fs->Filemap[num_file].inode.size = 0;
	fs->Filemap[num_file].inode.num_block = 0;
	fs->Filemap[num_file].inode.id = num_file;
	fs->Filemap[num_file].inode.flags = flags;
	fs->num_file++;
}

static ssize_t vfs_read(int fd, void *buf, size_t nbyte) {
	if (last_id == -1)
		perror("Error happened in process schedule.");
	file_t *File = &tlist[last_id].fdlist[fd];
	if (File->fd == -1)
		return -1;
	if (strcmp(File->mount->root, "/dev") == 0)
	{
		switch (File->inode->id) {
			case 0: //null
				return 0;
			case 1: //zero
				memcpy(buf, "0", 1);
				return 1;
			case 2: //random
				srand(0);
				int dex= rand();
				char s[10];
				itoa(dec, s);
				memcpy(buf, s, strlen(s));
				return strlen(s);
		}
	}

	if (File->inode.flags & RD_ONLY == 0)
		return 0;
	if (nbyte <= 0)
		return 0;
	else if (nbyte > File->inode->size)
		nbyte = File->inode->size;
	if (nbyte + File->offset > File->inode->num_block * PIECE_SIZE)
		nbyte = File->inode->num_block * PIECE_SIZE - File->offset;
	int f_block = File->offset / PIECE_SIZE;
	int l_block = (nbyte + File->offset) / PIECE_SIZE;
	void *dest = NULL;
	void *src = NULL;
	int len = 0;
	int r_len = nbyte;
	for (int i = f_block; i <= l_block; i++)
	{
		if (i == f_block && f_block == l_block)
		{
			dest = buf;
			src = File->inode->block[f_block] + File->offset % PIECE_SIZE;
			len = nbyte;
		}
		else if (i == f_block)
		{
			dest = buf;
			src = File->inode->block[f_block] + File->offset % PIECE_SIZE;
			len = PIECE_SIZE - File->offset % PIECE_SIZE;
		}
		else if (i == l_block)
		{
			src = File->inode->block[l_block];
			len = r_len;
		}
		else
		{
			src = File->inode->block[i];
			len = PIECE_SIZE;
		}
		memcpy(dest, src, len);
		r_len = r_len - len;
		dest = dest + len;
	}
}

static ssize_t vfs_write(int fd, void *buf, size_t nbyte) {
	if (last_id == -1)
		return -1;
	file_t *File = tlist[last_id].fdlist[fd];
	if (nbyte <= 0)
		return -1;
	//TODO: pretty complex!
}

static off_t vfs_lseek(int fd, off_t offset, int whence) {
	file_t *File = tlist[last_id].fdlist[fd];
	
	switch (whence)
	{
		case SEEK_SET:
			File->offset = offset;
			break;
		case SEEK_END:
			File->offset = File->inode.size + offset;
			break;
		case SEEK_CUR:
			File->offset += offset;
			break;
	}
	return File->offset;
}

static int vfs_close(int fd) {
	tlist[last_id].fdlist[fd] = -1;
	return 0;
}
