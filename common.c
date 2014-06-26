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

#include "modbus_utils.h"
#include <getopt.h>

int enable_debug;

enum modbus_type_e modbus_type = RTU;
struct modbus_parms_s modbus_parms = {
	/* RTU */
	{
		.serial_dev	= "/dev/ttyUSB0",
		.baud		= 115200,
		.bytes		= 8,
		.parity		= 'N',
		.stop		= 1
	},
	/* TCP */
	{
		.address	= "127.0.0.1",
		.port		= 1502
	}
};

/*
 * Local variables & functions
 */

static int parse_rtu_opts(char *opts)
{
	char *str;
	int ret;

	ret = sscanf(optarg, "%a[a-z]:%a[a-zA-Z0-9/_],%d,%d%c%d", &str,
		     &modbus_parms.rtu.serial_dev,
		     &modbus_parms.rtu.baud,
		     &modbus_parms.rtu.bytes,
		     &modbus_parms.rtu.parity, &modbus_parms.rtu.stop);
	free(str);
	if (ret < 1)
		return -1;

	return 0;
}

static int parse_tcp_opts(char *opts)
{
	char *str;
	int ret;

	ret = sscanf(optarg, "%a[a-z]:%a[a-zA-Z0-9/_],%d", &str,
		     &modbus_parms.tcp.address,
		     &modbus_parms.tcp.port);
	free(str);
	if (ret < 1)
		return -1;

	return 0;
}

static int parse_not_negative_int(char *addr)
{
	int val;
	int ret;

	ret = sscanf(addr, "0x%x", &val);
	if (ret < 1)
		ret = sscanf(addr, "%d", &val);
	if (ret < 1)
		return -1;

	return val;
}

/*
 * Exported functions
 */

void usage_common_opts(void)
{
	fprintf(stderr, "where <options> are:\n"
		"\t--debug  | -D        - enable debug messages (more increase verbosity)\n"
		"\t--device | -d <dev>  - specify MODBUS device to use\n"
		"\t                       <dev> can be:\n"
		"\t                       - rtu[:<ttydev>[,<baud>[,<bits><parity><stop>]]]\n"
		"\t                       - tcp[:<address>[,<port>]]\n"
		"\t--help   | -h        - show this help message\n");
}

int check_common_opts(int argc, char *argv[])
{
	int c;
	struct option long_options[] = {
		{"debug", no_argument, 0, 'D'},
		{"device", required_argument, 0, 'd'},
		{"help", no_argument, 0, 'h'},
	};
	char *str;
	int ret;

	/* Check the command line */
	while (1) {
		/* `getopt_long' stores the option index here. */
		int option_index = 0;

		c = getopt_long(argc, argv, "Dd:h",
				long_options, &option_index);

		/* Detect the end of the options. */
		if (c == -1)
			break;

		switch (c) {
		case 0:
			/* If this option set a flag, do nothing else now */
			BUG_ON(long_options[option_index].flag == NULL);

			break;

		case 'D':	/* --debug */
			enable_debug++;

			break;

		case 'd':	/* --device */
			ret = sscanf(optarg, "%a[a-z]", &str);
			if (ret != 1) {
				err("must specify a MODBUS type");
				exit(EXIT_FAILURE);
			}

			if (strcmp(str, "rtu") == 0) {
				free(str);
				modbus_type = RTU;

				ret = parse_rtu_opts(optarg);
				if (ret < 0) {
					err("invalid RTU options");
					exit(EXIT_FAILURE);
				}
			} else if (strcmp(str, "tcp") == 0) {
				free(str);
				modbus_type = TCP;

				ret = parse_tcp_opts(optarg);
				if (ret < 0) {
					err("invalid TCP options");
					exit(EXIT_FAILURE);
				}
			} else {
				err("unknow MODBUS type");
				exit(EXIT_FAILURE);
			}

			break;

		case 'h':	/* --help */
			usage();

		case ':':
			/* "getopt_long" already printed an error message */
			exit(EXIT_FAILURE);

		case '?':
			/* "getopt_long" already printed an error message */
			exit(EXIT_FAILURE);

		default:
			exit(EXIT_FAILURE);
		}
	}

	dbg("debug is on");
	switch (modbus_type) {
	case RTU:
		dbg("modbus rtu:%s,%d,%d,%c,%d",
		    modbus_parms.rtu.serial_dev,
		    modbus_parms.rtu.baud,
		    modbus_parms.rtu.bytes,
		    modbus_parms.rtu.parity, modbus_parms.rtu.stop);

		break;

	case TCP:
		dbg("modbus tcp:%s,%d",
		    modbus_parms.tcp.address,
		    modbus_parms.tcp.port);

		break;

	default:
		BUG();
	}

	return optind;
}

int parse_addr(char *addr)
{
	int val;

	val = parse_not_negative_int(addr);
	if (val < 1 || val > 254)
		return -1;

	return val;
}

int parse_reg(char *reg)
{
	int val;

	val = parse_not_negative_int(reg);
	if (val < 0 || val > 0xffff)
		return -1;

	return val;
}

int parse_datum(char *datum)
{
	int val;

	val = parse_not_negative_int(datum);
	if (val < 0 || val > 0xffff)
		return -1;

	return val;
}

modbus_t *modbus_client_connect(uint8_t addr)
{
	modbus_t *ctx;
	int ret;

	dbg("addr=%d", addr);

	switch (modbus_type) {
	case RTU:
		ctx = modbus_new_rtu(modbus_parms.rtu.serial_dev,
				     modbus_parms.rtu.baud,
				     modbus_parms.rtu.parity,
				     modbus_parms.rtu.bytes,
				     modbus_parms.rtu.stop);
		break;

	case TCP:
		ctx = modbus_new_tcp(modbus_parms.tcp.address,
				     modbus_parms.tcp.port);
		break;

	default:
		BUG();
	}
	if (!ctx) {
		err("MODBUS init error: %s", modbus_strerror(errno));
		goto exit;
	}

	ret = modbus_connect(ctx);
	if (ret == -1) {
		err("MODBUS connect error: %s", modbus_strerror(errno));
		goto free;
	}

	if (enable_debug >= DEBUG_VERBOSE)
		modbus_set_debug(ctx, 1);

	ret = modbus_set_slave(ctx, addr);
	if (ret == -1) {
		err("RTU connect error: %s", modbus_strerror(errno));
		goto close;
	}

	return ctx;

close:
	modbus_close(ctx);
free:
	modbus_free(ctx);
exit:
	return NULL;
}
