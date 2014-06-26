/*
 * Modbus utils
 *
 * Copyright (C) 2013-2014	Rodolfo Giometti <giometti@linux.it>
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

/*
 * Local functions
 */

void usage(void)
{
	fprintf(stderr, "usage: %s [<options>] <addr>\n", NAME);
	usage_common_opts();

	exit(EXIT_FAILURE);
}

/*
 * MAIN
 */

int main(int argc, char *argv[])
{
	int addr;
	modbus_t *ctx;
	modbus_mapping_t *mb_mapping;
	int ret;

	/*
	 * Check command line
	 */

	optind = check_common_opts(argc, argv);
	if (argc - optind < 1)
		usage();

	addr = parse_addr(argv[optind + 0]);
	if (addr < 1) {
		err("invalid address");
		exit(-1);
	}

	/*
	 * Modbus stuff
	 */

	ctx = modbus_server_connect(addr);
	if (!ctx) {
		err("connection failed: %s", modbus_strerror(errno));
		exit(1);
	}

	mb_mapping = modbus_mapping_new(MODBUS_MAX_READ_BITS, 0,
					MODBUS_MAX_READ_REGISTERS, 0);
	if (mb_mapping == NULL) {
		fprintf(stderr, "Failed to allocate the mapping: %s\n",
			modbus_strerror(errno));
		modbus_free(ctx);
		return -1;
	}

	for (;;) {
		uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];

		ret = modbus_receive(ctx, query);
		if (ret >= 0) {
			printf("Replying to request.\n");
			modbus_reply(ctx, query, ret, mb_mapping);
		} else {
			/* Connection closed by the client or server */
			break;
		}
	}

	printf("Quit the loop: %s\n", modbus_strerror(errno));

	modbus_mapping_free(mb_mapping);

	modbus_close(ctx);
	modbus_free(ctx);

	return 0;
}
