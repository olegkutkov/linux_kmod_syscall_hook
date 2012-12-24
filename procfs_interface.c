/* procfs_interface 
	- manage procfs file

Copyright 2012  Oleg Kutkov <elenbert@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.  
*/

#include <asm/uaccess.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include "procfs_interface.h"
#include "log.h"

static struct proc_dir_entry* procfs_entry_dir = NULL;
static struct proc_dir_entry* procfs_entry_file = NULL;
static io_procfs_t kmod_io = { NULL, NULL };

#define KMOD_READ_DATA 		kmod_io.procfs_read
#define KMOD_WRITE_DATA 	kmod_io.procfs_write

#define DATA_TRANSFER_SIZE_MAX	8

static int procfs_read(char* page, char** start, off_t off, int count, int* eof, void *data)
{
	return KMOD_READ_DATA(page);
}

static int procfs_write(struct file* file, const char* buffer, unsigned long count, void* data)
{
	int len;
	char str[DATA_TRANSFER_SIZE_MAX];

	if (count > DATA_TRANSFER_SIZE_MAX) {
		LOG_WARNING("To much data was writeen in proc, max=%d\n", DATA_TRANSFER_SIZE_MAX);
		return -ENOMEM;
	}

	len = DATA_TRANSFER_SIZE_MAX > count ? count : DATA_TRANSFER_SIZE_MAX;

	if (copy_from_user(str, buffer, len)) {
		LOG_ERROR("Unable to read data from user space\n");
		return -EFAULT;
	}

	str[len] = '\0';

	return KMOD_WRITE_DATA(str);
}

int create_procfs_entry(const char* dir, const char* entry, io_procfs_t* io)
{
	procfs_entry_dir = proc_mkdir(dir, NULL);

	if (!procfs_entry_dir) {
		LOG_ERROR("proc_mkdir() failed!\n");
		return -ENOMEM;
	}

	procfs_entry_file = create_proc_entry(entry, 0644, procfs_entry_dir);

	if (!procfs_entry_file) {
		LOG_ERROR("create_proc_entry() failed!");
		return -ENOMEM;
	}

	KMOD_READ_DATA = io->procfs_read;
	KMOD_WRITE_DATA = io->procfs_write;

	procfs_entry_file->read_proc = procfs_read;
	procfs_entry_file->write_proc = procfs_write;

	return 0;
}

void remove_procfs_entry(const char* dir, const char* entry)
{
	if (procfs_entry_dir) {
		remove_proc_entry(entry, procfs_entry_dir);
		remove_proc_entry(dir, NULL);
	}
}

