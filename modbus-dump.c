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

/*
 * Local functions
 */

void usage(void)
{
	fprintf(stderr, "usage: %s [<options>] <addr> <start> <end>\n", NAME);
	usage_common_opts();

	exit(EXIT_FAILURE);
}

/*
 * MAIN
 */

int main(int argc, char *argv[])
{
	int addr;
	int r_start, r_end;
	uint16_t *regs;
	modbus_t *ctx;
	int i, ret;

	/*
	 * Check command line
	 */

	optind = check_common_opts(argc, argv);
	if (argc - optind < 3)
		usage();

	addr = parse_addr(argv[optind + 0]);
	if (addr < 1) {
		err("invalid address");
		exit(-1);
	}

	r_start = parse_reg(argv[optind + 1]);
	if (r_start < 0) {
		err("invalid start register");
		exit(-1);
	}

	r_end = parse_reg(argv[optind + 2]);
	if (r_end < 0) {
		err("invalid stop register");
		exit(-1);
	}

	if (r_end - r_start + 1 <= 0) {
		err("invalid registers range");
		exit(-1);
	}

	regs = malloc(sizeof(uint16_t) * (r_end - r_start + 1));
	if (!regs) {
		err("out of memory!");
		exit(-1);
	}

	/*
	 * Modbus stuff
	 */

	ctx = modbus_client_connect(addr);
	if (!ctx) {
		err("connection failed: %s", modbus_strerror(errno));
		exit(1);
	}

	ret = modbus_read_registers(ctx, r_start, r_end - r_start + 1, regs);
	if (ret == -1) {
		err("read error: %s", modbus_strerror(errno));
		exit(-1);
	}

	for (i = 0; i < r_end - r_start + 1; i++)
		printf("%s: reg[%d]=%d/0x%04x\n", NAME, r_start + i,
		       regs[i], regs[i]);

	free(regs);
	modbus_close(ctx);
	modbus_free(ctx);

	return 0;
}
