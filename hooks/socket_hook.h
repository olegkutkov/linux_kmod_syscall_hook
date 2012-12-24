/* hooks/socket_hook.h
	- hooks for socket functions

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

#ifndef SYSHOOK_SOCKET_HOOK_H
#define SYSHOOK_SOCKET_HOOK_H

#include <linux/syscalls.h>

#if defined (__arm__) || defined (__x86_64__)
	#define SOCKET_CALL  __NR_socket
	#define SOCKET_CALL_ARGS int family, int type, int protocol
	#define SOCKET_ONLY
#else
	#define SOCKET_CALL __NR_socketcall
	#define SOCKET_CALL_ARGS int call, unsigned long __user *args
#endif

asmlinkage long new_socketcall(SOCKET_CALL_ARGS);

#ifdef SOCKET_ONLY
asmlinkage long new_sock_connect(int fd, struct sockaddr __user* uservaddr, int addrlen);
asmlinkage long new_sock_bind(int fd, struct sockaddr __user* umyaddr, int addrlen);
asmlinkage long new_sock_setsockopt(int fd, int level, int optname, char __user* optval, int optlen);
#endif

#endif // SYSHOOK_SOCKET_HOOK_H

