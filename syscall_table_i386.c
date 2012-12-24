/* syscall_table_i386.c
	- find syscall table on the Intel 32-bit platform

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
#include <linux/string.h>

/* 
 * http://phrack.org/archives/58/p58-0x07
 * Volume 0x0b, Issue 0x3a, Phile #0x07 of 0x0e
 * Linux on-the-fly kernel patching without LKM
 */
struct
{
	unsigned short limit;
	unsigned int base;
} __attribute__ ((packed)) idtr;

struct
{
	unsigned short off1;
	unsigned short sel;
	unsigned char none, flags;
	unsigned short off2;
} __attribute__ ((packed)) idt;


void* memmem(const void *haystack, size_t haystacklen, const void *needle, size_t needlelen)
{
	if (   !haystack 
		|| !haystacklen
		|| !needle 
		|| !needlelen
		|| haystacklen < needlelen) {
		return NULL;
	}

	while (   haystacklen >= needlelen
			&& haystacklen-- 
			&& memcmp(haystack, needle, needlelen)) {
		haystack++;
	}

	return (haystacklen >= needlelen) ? (void*) haystack : NULL;
}

unsigned** find_syscall_table(void)
{
	const int CALLOFF = 100;
	unsigned sys_call_off;
	unsigned sct = 0;
	char sc_asm[CALLOFF], *p;
	
	asm ("sidt %0" : "=m" (idtr));

	memcpy(&idt, (char*) (idtr.base + 8*0x80), sizeof(idt));
	sys_call_off = (idt.off2 << 16) | idt.off1;

	memcpy(sc_asm, (char*) sys_call_off, CALLOFF);
	p = (char*) memmem(sc_asm, CALLOFF, "\xff\x14\x85", 3);

	if (p) {
		sct = *(UNSIGNED*) (p + 3);
	}

	return (unsigned**) sct;
}


