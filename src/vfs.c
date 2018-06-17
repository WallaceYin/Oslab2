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

const char cpuinfo[] = "cpu family : 6\nmodel : 142\nmodel name : Intel(R) Core(TM) i5-7300U CPU @ 2.60GHz\n";
const char meminfo[] = "_heap = [00500000, 08000000)\n";

static void vfs_init() {
	procfs = (filesystem_t *)pmm->alloc(sizeof(filesystem_t));
	devfs = (filesystem_t *)pmm->alloc(sizeof(filesystem_t));
	kvfs = (filesystem_t *)pmm->alloc(sizeof(filesystem_t));
	vfs->mount("/proc", procfs);
	vfs->mount("/dev", devfs);
	vfs->mount("/", kvfs);
	
	vfs->create(procfs, "/proc/cpuinfo", O_RDONLY);
	vfs->create(procfs, "/proc/meminfo", O_RDONLY);
	vfs->create(devfs, "/dev/null", O_RDWR);
	vfs->create(devfs, "/dev/zero", O_RDWR);
	vfs->create(devfs, "/dev/random", O_RDWR);
	
}

static int vfs_access(const char *path, int mode) {
	//TODO
	return 1;
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
	if (strstr(path, "/proc") != NULL)
		fs = procfs;
	else if (strstr(path, "/dev") != NULL)
		fs = devfs;
	else if (strstr(path, "/") != NULL)
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
	strcpy(fs->Filemap[fs->num_file].path, path);
	fs->Filemap[fs->num_file].inode.size = 0;
	fs->Filemap[fs->num_file].inode.num_block = 0;
	fs->Filemap[fs->num_file].inode.id = fs->num_file;
	fs->Filemap[fs->num_file].inode.flags = flags;
	for (int i = 0; i < MAX_BLOCK; i++)
		fs->Filemap[fs->num_file].inode.block[i] = NULL;
	if (strcmp("/proc/cpuinfo", path) == 0)
	{
		fs->Filemap[fs->num_file].inode.num_block = 1;
		fs->Filemap[fs->num_file].inode.block[0] = (void *)pmm->alloc(PIECE_SIZE);
		memcpy(fs->Filemap[fs->num_file].inode.block[0], cpuinfo, strlen(cpuinfo));
	}
	else if (strcmp("/proc/meminfo", path) == 0)
	{
		fs->Filemap[fs->num_file].inode.num_block = 1;
		fs->Filemap[fs->num_file].inode.block[0] = (void *)pmm->alloc(PIECE_SIZE);
		memcpy(fs->Filemap[fs->num_file].inode.block[0], meminfo, strlen(meminfo));
	}
	else if (strstr(path, "/proc") != NULL)
	{
		fs->Filemap[fs->num_file].inode.num_block = 1;
		fs->Filemap[fs->num_file].inode.block[0] = (void *)pmm->alloc(PIECE_SIZE);
		char *pidstr = strcut(path, strlen("/proc/"));
		char pidinfo[50];
		strcpy(pidinfo, "Pid: ");
		strcat(pidinfo, pidstr);
		strcat(pidinfo, "\nStatus: Running\n");
		memcpy(fs->Filemap[fs->num_file].inode.block[0],pidinfo, strlen(pidinfo));
	}
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
				itoa(dex, s);
				memcpy(buf, s, strlen(s));
				return strlen(s);
		}
	}

	if ((File->inode->flags & O_RDONLY) == 0)
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
	return nbyte;
}

static ssize_t vfs_write(int fd, void *buf, size_t nbyte) {
	if (last_id == -1)
	{
		perror("Error happened in process schedule.");
		return -1;
	}
	file_t *File = &tlist[last_id].fdlist[fd];

	if (nbyte <= 0)
		return -1;
	if (File->fd == -1)
		return -1;

	if (strcmp(File->mount->root, "/dev") == 0)
		return 0;
	
	else if ((File->inode->flags & O_WRONLY) == 0)
		return -1;
	
	int blks = File->inode->num_block;
	while (blks < (nbyte + File->offset) / PIECE_SIZE + 1)
	{
		File->inode->block[blks] = (void *)pmm->alloc(PIECE_SIZE);
		blks ++;
	}
	File->inode->num_block = blks;
	File->inode->size += nbyte;
	int l_blk = File->offset / PIECE_SIZE;
	int r_blk = (File->offset + nbyte) / PIECE_SIZE;
	//void *dest = File->inode->block[l_blk] + File->offset % PIECE_SIZE;
	void *dest = NULL;
	void *src = NULL;
	int len = 0;
	int u_len = 0;
	for (int i = l_blk; i <= r_blk; i++)
	{
		if (l_blk == r_blk)
		{
			dest = File->inode->block[l_blk] + File->offset % PIECE_SIZE;
			src = buf;
			len = nbyte;
		}
		else if (i == l_blk)
		{
			dest = File->inode->block[l_blk] + File->offset % PIECE_SIZE;
			src = buf;
			len = PIECE_SIZE - File->offset % PIECE_SIZE;
		}
		else if (i == r_blk)
		{
			dest = File->inode->block[r_blk];
			src = buf + u_len;
			len = nbyte - u_len;
		}
		else
		{
			dest = File->inode->block[i];
			src = buf + u_len;
			len = PIECE_SIZE;
		}
		memcpy(dest, src, len);
		u_len += len;
	}
	return nbyte;
}

static off_t vfs_lseek(int fd, off_t offset, int whence) {
	file_t *File = &tlist[last_id].fdlist[fd];
	
	switch (whence)
	{
		case SEEK_SET:
			File->offset = offset;
			break;
		case SEEK_END:
			File->offset = File->inode->size + offset;
			break;
		case SEEK_CUR:
			File->offset += offset;
			break;
	}
	return File->offset;
}

static int vfs_close(int fd) {
	tlist[last_id].fdlist[fd].fd = -1;
	return 0;
}
