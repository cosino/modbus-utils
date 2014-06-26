/*
 * Modbus utils
 *
 * Copyright (C) 2013	Rodolfo Giometti <giometti@linux.it>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _MODBUS_UTILS_H
#define _MODBUS_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <modbus.h>

/*
 * Misc macros
 */

#define NAME		    program_invocation_short_name

#define info(fmt, args...)						\
		printf(fmt "\n" , ## args)

#define warn(fmt, args...)						\
		printf(fmt "\n" , ## args)

#define err(fmt, args...)						\
		fprintf(stderr, fmt "\n" , ## args)

#define dbg(fmt, args...)						\
	do {								\
		if (unlikely(enable_debug))				\
			fprintf(stderr, "%s[%4d]: %s: " fmt "\n" ,	\
				__FILE__, __LINE__, __func__ , ## args);	\
	} while (0)

#define __deprecated	    __attribute__ ((deprecated))
#define __packed		__attribute__ ((packed))
#define __constructor	   __attribute__ ((constructor))

#define unlikely(x)	  __builtin_expect(!!(x), 0)
#define BUG()								\
	do {								\
		err("fatal error in %s():%d", __func__, __LINE__);	\
		exit(EXIT_FAILURE);					\
	} while (0)
#define EXIT_ON(condition)						\
	do {								\
		if (unlikely(condition))				\
			BUG();						\
	} while(0)
#define BUG_ON(condition)       EXIT_ON(condition)

#define WARN()								\
	do {								\
		err("warning notice in %s():%d", __func__, __LINE__);	\
	} while (0)
#define WARN_ON(condition)						\
	do {								\
		if (unlikely(condition))				\
			WARN();						\
	} while(0)

/*
 * Global types
 */

enum modbus_dbg_lev_e {
	DEBUG_NONE,
	DEBUG,
	DEBUG_VERBOSE,
};

enum modbus_type_e {
	RTU,
	TCP,
	__TYPE_ERROR
};

struct modbus_parms_s {
	struct modbus_rtu_parms_s {
		char *serial_dev;
		int baud;
		int bytes;
		char parity;
		int stop;
	} rtu;

	struct modbus_tcp_parms_s {
		char *address;
		int port;
	} tcp;
};

/*
 * Exported variables & functions
 */

extern int enable_debug;

extern enum modbus_type_e modbus_type;
extern struct modbus_parms_s modbus_parms;

extern void usage(void);
extern void usage_common_opts(void);
extern int check_common_opts(int argc, char *argv[]);

extern int parse_addr(char *addr);
extern int parse_reg(char *reg);
extern int parse_datum(char *datum);

extern modbus_t *modbus_client_connect(uint8_t addr);
extern modbus_t *modbus_server_connect(uint8_t addr);

#endif /* _MODBUS_UTILS_H */
