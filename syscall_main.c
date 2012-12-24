/* syscall_main.c
	- kernel module entry point

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

#include <linux/kernel.h>
#include <linux/module.h>
#include "procfs_interface.h"
#include "info.h"
#include "log.h"
#include "hook_install.h"

#define FILE_SWITCH_NAME "set_syscall_hooks"

static int __init syshook_load(void)
{
	int res = 0;
	io_procfs_t io = { get_hook_state, set_hook_state };

	LOG_INFO("Loading...\n");

	res = create_procfs_entry(KMOD_NAME, FILE_SWITCH_NAME, &io);

	if (res != 0) {
		LOG_ERROR("Failed to create procfs entry, error=%d\n", res);
	}

	return res;
}

static void __exit syshook_unload(void)
{
	LOG_INFO("Unloading...\n");

	remove_procfs_entry(KMOD_NAME, FILE_SWITCH_NAME);
	set_hook_state("0");
}

MODULE_DESCRIPTION (KMOD_DESCRIPTION);
MODULE_VERSION (KMOD_VERSION);
MODULE_AUTHOR (KMOD_AUTHOR);
MODULE_LICENSE (KMOD_LICENSE);

module_init(syshook_load);
module_exit(syshook_unload);

