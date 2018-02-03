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

enum modbus_data_e {
	MODBUS_BITS,
	MODBUS_INPUT_BITS,
	MODBUS_REGISTERS,
};

/*
 * Local functions
 */

void usage(void)
{
	fprintf(stderr,
		"usage: %s [<options>] <addr> <type> <start> <end>\n", NAME);
	fprintf(stderr, "\t[ <type> <start> <end> ... ]\n");
	fprintf(stderr, "where <type> can be:\n");
	fprintf(stderr,
		"\t- \"bits\", \"coil\" or \"obits\" for output bits\n");
	fprintf(stderr, "\t- \"ibits\" for input bits\n");
	fprintf(stderr, "\t- \"registers\" for registers\n");
	usage_common_opts();

	exit(EXIT_FAILURE);
}

void do_dump(modbus_t *ctx, char *type, char *start, char *end)
{
	enum modbus_data_e mode;
	int r_start, r_end;
	uint8_t *regs8 = NULL;
	uint16_t *regs16 = NULL;
	int i, ret;

	if ((strncmp(type, "bits", strlen(type)) == 0) ||
	    (strncmp(type, "obits", strlen(type)) == 0) ||
	    (strncmp(type, "coils", strlen(type)) == 0))
		mode = MODBUS_BITS;
	else if (strncmp(type, "ibits", strlen(type)) == 0)
		mode = MODBUS_INPUT_BITS;
	else if (strncmp(type, "registers", strlen(type)) == 0)
		mode = MODBUS_REGISTERS;
	else {
		err("invalid type \"%s\"", type);
		exit(-1);
	}

	r_start = parse_reg(start);
	if (r_start < 0) {
		err("invalid start register \"%s\"", start);
		exit(-1);
	}

	r_end = parse_reg(end);
	if (r_end < 0) {
		err("invalid stop register \"%s\"", end);
		exit(-1);
	}

	if (r_end - r_start + 1 <= 0) {
		err("invalid registers range \"[%s,%s]\"", start, end);
		exit(-1);
	}

	switch (mode) {
	case MODBUS_BITS:
	case MODBUS_INPUT_BITS:
		regs8 = malloc(sizeof(uint8_t) * (r_end - r_start + 1));
		if (!regs8) {
			err("out of memory!");
			exit(-1);
		}

		break;

	case MODBUS_REGISTERS:
		regs16 = malloc(sizeof(uint16_t) * (r_end - r_start + 1));
		if (!regs16) {
			err("out of memory!");
			exit(-1);
		}

		break;
	}

	/*
	 * Modbus stuff
	 */

	switch (mode) {
	case MODBUS_BITS:
		ret = modbus_read_bits(ctx, r_start,
					r_end - r_start + 1, regs8);
		if (ret == -1) {
			err("read error: %s", modbus_strerror(errno));
			exit(-1);
		}
	
		for (i = 0; i < r_end - r_start + 1; i++)
			printf("%s: obit[%d]=%d/0x%02x\n", NAME, r_start + i,
			       regs8[i], regs8[i]);

		break;

	case MODBUS_INPUT_BITS:
		ret = modbus_read_input_bits(ctx, r_start,
					r_end - r_start + 1, regs8);
		if (ret == -1) {
			err("read error: %s", modbus_strerror(errno));
			exit(-1);
		}
	
		for (i = 0; i < r_end - r_start + 1; i++)
			printf("%s: ibit[%d]=%d/0x%02x\n", NAME, r_start + i,
			       regs8[i], regs8[i]);

		break;

	case MODBUS_REGISTERS:
		ret = modbus_read_registers(ctx, r_start,
					r_end - r_start + 1, regs16);
		if (ret == -1) {
			err("read error: %s", modbus_strerror(errno));
			exit(-1);
		}
	
		for (i = 0; i < r_end - r_start + 1; i++)
			printf("%s: reg[%d]=%d/0x%04x\n", NAME, r_start + i,
			       regs16[i], regs16[i]);
	
		break;
	}

	free(regs8);
	free(regs16);
}

/*
 * MAIN
 */

int main(int argc, char *argv[])
{
	int addr;
	modbus_t *ctx;
	int n;

	/*
	 * Check command line
	 */

	optind = check_common_opts(argc, argv);
	if ((argc - optind < 4) || ((argc - optind - 1) % 3))
		usage();

	addr = parse_addr(argv[optind + 0]);
	if (addr < 0) {
		err("invalid address \"%s\"", argv[optind + 0]);
		exit(-1);
	}

	ctx = modbus_client_connect(addr);
	if (!ctx) {
		err("connection failed: %s", modbus_strerror(errno));
		exit(1);
	}

	for (n = 0; n < (argc - optind - 1) / 3; n++)
		do_dump(ctx, argv[optind + n * 3 + 1],
			     argv[optind + n * 3 + 2],
			     argv[optind + n * 3 + 3]);

	modbus_close(ctx);
	modbus_free(ctx);

	return 0;
}
