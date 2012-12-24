/* syscall_table_arm32.c
	- find syscall table on the ARM 32-bit platform

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

#include <linux/init_task.h>
#include <linux/syscalls.h>
#include <linux/unistd.h>
#include "utils.h"

unsigned long** find_syscall_table(void)
{
   	unsigned long** sctable;
   	unsigned long ptr;
   	extern unsigned long loops_per_jiffy;

	sctable = NULL;

	for (ptr = (unsigned long)&elf_check_arch;
			ptr < (unsigned long)&loops_per_jiffy; 
			ptr += sizeof(void*)
	) {
		unsigned long *p = (unsigned long*)ptr;

		if (p[__NR_close] == (unsigned long) sys_close) {
			sctable = (unsigned long**)p;
			return &sctable[0];
		}
	}

	return NULL;
}

