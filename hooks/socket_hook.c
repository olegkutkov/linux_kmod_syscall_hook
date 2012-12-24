/* hooks/socket_hook.c
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

#include "log.h"
#include "hook_install.h"
#include "hooks/socket_hook.h"


typedef asmlinkage long (*socketcall_func_ptr) (SOCKET_CALL_ARGS);
typedef asmlinkage long (*socket_connect_func_ptr) (int fd, struct sockaddr __user* uservaddr, int addrlen);
typedef asmlinkage long (*socket_bind_func_ptr) (int fd, struct sockaddr __user* umyaddr, int addrlen);
typedef asmlinkage long (*socket_setsockopt_func_ptr) (int fd, int level, int optname, char __user* optval, int optlen);


#ifdef SOCKET_ONLY
asmlinkage long new_socketcall(int family, int type, int protocol)
{
	int ret;

	LOG_INFO("new_socketcall() invoked!\n");

	ORIGINAL_SYSCALL_3(SOCKET_CALL, socketcall_func_ptr, family, type, protocol, ret);

	return ret;
}
#else
asmlinkage long new_socketcall(int call, unsigned long __user *args)
{
	int ret;

	LOG_INFO("new_socketcall() invoked!\n");

	ORIGINAL_SYSCALL_2(SOCKET_CALL, socketcall_func_ptr, call, args, ret);

	return ret;
}
#endif

asmlinkage long new_sock_connect(int fd, struct sockaddr __user* uservaddr, int addrlen)
{
	int ret;

	LOG_INFO("new_sock_connect() invoked!\n");

	ORIGINAL_SYSCALL_3(__NR_connect, socket_connect_func_ptr, fd, uservaddr, addrlen, ret);

	return ret;
}

asmlinkage long new_sock_bind(int fd, struct sockaddr __user* umyaddr, int addrlen)
{
	int ret;

	LOG_INFO("new_sock_bind() invoked!\n");

	ORIGINAL_SYSCALL_3(__NR_bind, socket_bind_func_ptr, fd, umyaddr, addrlen, ret);

	return ret;
}

asmlinkage long new_sock_setsockopt(int fd, int level, int optname, char __user* optval, int optlen)
{
	int ret;

	LOG_INFO("new_sock_setsockopt() invoked\n");

	ORIGINAL_SYSCALL_5(__NR_setsockopt, socket_setsockopt_func_ptr, fd, level, optname, optval, optlen, ret);

	return ret;
}

