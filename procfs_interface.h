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

#ifndef SYSHOOK_PROCFS_INTERFACE_H
#define SYSHOOK_PROCFS_INTERFACE_H

struct io_procfs {
	int (*procfs_read) (char* data);
	int (*procfs_write) (char* data);
};
typedef struct io_procfs io_procfs_t;


int create_procfs_entry(const char* dir, const char* entry, io_procfs_t* io);
void remove_procfs_entry(const char* dir, const char* entry);

#endif

