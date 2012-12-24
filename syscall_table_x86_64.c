/* syscall_table_x86_64.c
	- find syscall table on the Intel 64-bit platform

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

#include <linux/types.h>
#include <asm/msr-index.h>

unsigned long** find_syscall_table(void)
{
	int i, lo, hi;
	unsigned char *ptr;
	unsigned long system_call;

	asm volatile("rdmsr" : "=a" (lo), "=d" (hi) : "c" (MSR_LSTAR));

	system_call = (void*)(((long)hi << 32) | lo);

	for (ptr = system_call, i = 0; i < 500; i++)  {
		if (ptr[0] == 0xff && ptr[1] == 0x14 && ptr[2] == 0xc5) {
			return (unsigned long**)(0xffffffff00000000 | *((unsigned int*)(ptr+3)));
		}

		ptr++;
	}

	return NULL;
}


