/* hook_install
	- install/remove syscall hooks

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

#include <linux/syscalls.h>
#include <linux/string.h>
#include <linux/version.h>
#include "hook_install.h"
#include "log.h"
#include "hooks/socket_hook.h"

#if defined (__i386__) 
	#define UNSIGNED	unsigned int
#elif defined (__x86_64__) || defined (__arm__)
	#define UNSIGNED	unsigned long 
#endif

struct syscall_hooks {
	UNSIGNED syscall;
	void* hook_ptr;
	void* orign_ptr;
};
typedef struct syscall_hooks syscall_hooks_t;

static int hook_state = 0;
static UNSIGNED** sys_call_table = NULL;

/*
 * Hooks table:
 *  .syscall - syscall number from kernel
 *  .hook_ptr - address of the our hook function
 *  .orign_ptr - address of the original system call function
 *
 *  Add your custom hooks in the end of this table
 *  Table should be terminated with zeros
 */
static syscall_hooks_t hooks_table[] = {
	{ SOCKET_CALL, new_socketcall, 0 },
#ifdef SOCKET_ONLY
	{ __NR_connect, new_sock_connect, 0 },
	{ __NR_bind, new_sock_bind, 0 },
	{ __NR_setsockopt, new_sock_setsockopt, 0 },
#endif
	{ 0, 0, 0 }
};

/*
 * On Linux kernels, newer than 2.6.29, is used CPU memory protection
 *  to avoid modifications of syscall table.
 * Here we do some hack, playing with 16 bit of CR0 register, this can disable
 *  and enable memory protection
 *
 * Such protection doesn't exist on modern ARM cpu's, so we can easily
 *  overwrite pointers in syscall table
 */
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,29))
	#ifndef __arm__
		#define HACK_MEMORY_PROTECTION
		#if defined(__i386__)
			#define WP_CR0_FLAG 0x00010000		/* bit to change in 32-bit register */
		#elif defined(__x86_64__)
			#define WP_CR0_FLAG 0x0000000000010000  /* bit to change in 64-bit register */
		#endif
	#endif
#endif

#if defined(__i386__)
	#include "syscall_table_i386.c"
#elif defined(__x86_64__)
	#include "syscall_table_x86_64.c"
#elif defined(__arm__)
	#include "syscall_table_arm32.c"
#else
	#error "Your platform is not supported!"
#endif


void* get_syscall_original_ptr(unsigned long callnum)
{
	int i = 0;

	for (;;) {
		if (!hooks_table[i].syscall) {
			return NULL;
		}

		if (hooks_table[i].syscall == callnum) {
			return hooks_table[i].orign_ptr;
		}

		i++;
	};

	return NULL;
}

#ifdef HACK_MEMORY_PROTECTION
static unsigned long __asm_op_force_order;

/*
 * Get CR0 register value using x86 assembly
 */
static unsigned long cr0_register_get_value(void)
{
	unsigned long value;
#if defined(__i386__)
	asm volatile("mov %%cr0,%0\n\t" : "=r" (value), "=m" (__asm_op_force_order));
#elif defined(__x86_64__)
	asm volatile("movq %%cr0,%0\n\t" : "=r" (value), "=m" (__asm_op_force_order));
#endif
	return value;
}

/*
 * Set CR0 register value using x86 assembly
 */
static void cr0_register_set_value(unsigned long value)
{
#if defined(__i386__)
	asm volatile("mov %0,%%cr0": : "r" (value), "m" (__asm_op_force_order));
#elif defined(__x86_64__)
	asm volatile("movq %0,%%cr0" :: "r" (value), "m" (__asm_op_force_order));
#endif
}

/*
 * Unset 16-bit in CR0 register.
 * This should disable memory protection
 */
static int disable_memory_write_protection(void)
{
	/*
	* Instructions should be executed in correct order 
	* and we shouldn't wake up on another CPU/core during procedure
	*/
	unsigned long cr0;
	preempt_disable();
	barrier();

	cr0 = cr0_register_get_value() ^ WP_CR0_FLAG;

	if (unlikely(cr0 & WP_CR0_FLAG)) {
		LOG_WARNING("CR0 register is in incorrect state!\n");

		barrier();
		preempt_enable_no_resched();

		return -1;
	}

	cr0_register_set_value(cr0);

	return 0;
}

/*
 * Set 16-bit in CR0 register.
 * This should enable memory protection
 */
static int enable_memory_write_protection(void)
{
	unsigned long cr0 = cr0_register_get_value() ^ WP_CR0_FLAG;

	preempt_disable();
	barrier();

	if (unlikely(!(cr0 & WP_CR0_FLAG))) {
		LOG_WARNING("CR0 register is in incorrect state!\n");

		barrier();
		preempt_enable_no_resched();

		return -1;
	}

	cr0_register_set_value(cr0);

	barrier();
	preempt_enable_no_resched();

	return 0;
}
#endif

static int set_hooks(void)
{
	int i = 0;

	LOG_INFO("Hooking syscall table...\n");

	if (!sys_call_table) {
		sys_call_table = find_syscall_table();

		if (!sys_call_table) {
			LOG_ERROR("Unable to find system call table!\n");
			return -EFAULT;
		}
	}

#ifdef HACK_MEMORY_PROTECTION
	if (disable_memory_write_protection() == -1) {
		LOG_ERROR("Failed to disable memory protection. Unable to set hooks\n");
		return -EFAULT;
	}
#endif

	for (;;) {
		if (!hooks_table[i].syscall) {
			break;
		}

		hooks_table[i].orign_ptr = sys_call_table[hooks_table[i].syscall];

		sys_call_table[hooks_table[i].syscall] = hooks_table[i].hook_ptr;

		LOG_INFO("SYSCALL #%d original addr=%08X\n", hooks_table[i].syscall, hooks_table[i].orign_ptr);
		LOG_INFO("SYSCALL #%d new addr=%08X\n", hooks_table[i].syscall, hooks_table[i].hook_ptr);
		i++;
	};

#ifdef HACK_MEMORY_PROTECTION
	if (enable_memory_write_protection() == -1) {
		LOG_WARNING("Failed to enable memory protection. Probably it's better to reboot your computer\n");
	}
#endif

	return 0;
}

static int unset_hooks(void)
{
	int i = 0;

	LOG_INFO("Unhooking syscall table...\n");

#ifdef HACK_MEMORY_PROTECTION
	if (disable_memory_write_protection() == -1) {
		LOG_ERROR("Failed to disable memory protection. Unable to reset original syscalls. Please, reboot your computer!\n");
		return -EFAULT;
	}
#endif

	for (;;) {
		if (!hooks_table[i].syscall) {
			break;
		}

		sys_call_table[hooks_table[i].syscall] = hooks_table[i].orign_ptr;

		LOG_INFO("SYSCALL #%d restored original addr=%08X\n", hooks_table[i].syscall, sys_call_table[hooks_table[i].syscall]);
		i++;
	};

#ifdef HACK_MEMORY_PROTECTION
	if (enable_memory_write_protection() == -1) {
		LOG_WARNING("Failed to enable memory protection. Probably it's better to reboot your computer\n");
	}
#endif

	return 0;
}

int set_hook_state(char* data)
{
	if (data[0] == '1' && hook_state == 0 && set_hooks() == 0) {
		hook_state = 1;
	} else if (data[0] == '0' && hook_state == 1 && unset_hooks() == 0) {
		hook_state = 0;
	}

	return strlen(data);
}

int get_hook_state(char* data)
{
	char* result = hook_state ? "1\n" : "0\n";
	int len = strlen(result);

	strncpy(data, result, len);

	return len;
}

