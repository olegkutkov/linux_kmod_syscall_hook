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

#ifndef SYSHOOK_HOOK_INSTALL_H
#define SYSHOOK_HOOK_INSTALL_H

int set_hook_state(char* data);
int get_hook_state(char* data);

void* get_syscall_original_ptr(unsigned long callnum);

/*
 * Helper macros.
 * Wrappers to call original system call with 2 and 3 arguments
 */

#ifndef ORIGINAL_SYSCALL_2
#define ORIGINAL_SYSCALL_2(syscall, prototype, arg1, arg2, ret) { \
	prototype func = (prototype)get_syscall_original_ptr(syscall); \
						\
	if (func) { 						\
		ret = func(arg1, arg2);				\
	} else {					\
		ret = -1;			\
	} \
}
#endif

#ifndef ORIGINAL_SYSCALL_3
#define ORIGINAL_SYSCALL_3(syscall, prototype, arg1, arg2, arg3, ret)	{ \
	prototype func = (prototype)get_syscall_original_ptr(syscall); \
							\
	if (func) { 						\
		ret = func(arg1, arg2, arg3); 			\
	} else {					\
		ret = -1;			\
	} \
}
#endif

#ifndef ORIGINAL_SYSCALL_5
#define ORIGINAL_SYSCALL_5(syscall, prototype, arg1, arg2, arg3, arg4, arg5, ret) { \
	prototype func = (prototype)get_syscall_original_ptr(syscall); \
							\
	if (func) { 						\
		ret = func(arg1, arg2, arg3, arg4, arg5);	\
	} else {					\
		ret = -1;			\
	} \
}
#endif

#endif //SYSHOOK_HOOK_INSTALL_H

