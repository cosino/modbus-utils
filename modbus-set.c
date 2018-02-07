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
	MODBUS_REGISTERS,
};

/*
 * Local functions
 */

void usage(void)
{
	fprintf(stderr,
		"usage: %s [<options>] <addr> <type> <start> <val>\n", NAME);
	fprintf(stderr, "\t[<val> ...]\n");
	fprintf(stderr, "where <type> can be:\n");
	fprintf(stderr,
		"\t- \"bits\", \"coil\" or \"obits\" for output bits\n");
	fprintf(stderr,
		"\t- \"registers\", \"holding\" or \"oregisters\" for "
		"output registers\n");
	usage_common_opts();

	exit(EXIT_FAILURE);
}

/*
 * MAIN
 */

int main(int argc, char *argv[])
{
	int optind;
	int addr;
	char *type;
	enum modbus_data_e mode;
	int r_start;
	uint8_t *regs8 = NULL;
	uint16_t *regs16 = NULL;
	modbus_t *ctx;
	int i, val, ret;

	/*
	 * Check command line
	 */

	optind = check_common_opts(argc, argv);
	if (argc - optind < 3)
		usage();

	addr = parse_addr(argv[optind + 0]);
	if (addr < 1) {
		err("invalid address \"%s\"", argv[optind + 0]);
		exit(-1);
	}

	ctx = modbus_client_connect(addr);
	if (!ctx) {
		err("modbud connection failed: %s", modbus_strerror(errno));
		exit(1);
	}

	type = argv[optind + 1];
	if ((strncmp(type, "bits", strlen(type)) == 0) ||
	    (strncmp(type, "obits", strlen(type)) == 0) ||
	    (strncmp(type, "coils", strlen(type)) == 0))
		mode = MODBUS_BITS;
	else if ((strncmp(type, "registers", strlen(type)) == 0) ||
		 (strncmp(type, "oregisters", strlen(type)) == 0) ||
		 (strncmp(type, "holding", strlen(type)) == 0))
		mode = MODBUS_REGISTERS;
	else {
		err("invalid type \"%s\"", type);
		exit(-1);
	}

	r_start = parse_reg(argv[optind + 2]);
	if (r_start < 0) {
		err("invalid start register \"%s\"", argv[optind + 2]);
		exit(-1);
	}

	switch (mode) {
	case MODBUS_BITS:
		regs8 = malloc(sizeof(uint8_t) * (argc - optind - 3));
		if (!regs8) {
			err("out of memory!");
			exit(-1);
		}

		break;

	case MODBUS_REGISTERS:
		regs16 = malloc(sizeof(uint16_t) * (argc - optind - 3));
		if (!regs16) {
			err("out of memory!");
			exit(-1);
		}

		break;
	}

	for (i = 0; i < argc - optind - 3; i++) {
		val = parse_datum(argv[optind + 3 + i]);
		if (val < 0) {
			err("invalid value \"%s\" at position %d",
						argv[optind + 3 + i], i);
			exit(-1);
		}
		switch (mode) {
		case MODBUS_BITS:
			regs8[i] = val;
			break;

		case MODBUS_REGISTERS:
			regs16[i] = val;
			break;
		}
	}

	/*
	 * Modbus stuff
	 */

	switch (mode) {
	case MODBUS_BITS:
		ret = modbus_write_bits(ctx, r_start ,
					argc - optind - 3, regs8);
		break;
	case MODBUS_REGISTERS:
		ret = modbus_write_registers(ctx, r_start,
					argc - optind - 3, regs16);
		break;
	}
	if (ret == -1) {
		err("write error: %s", modbus_strerror(errno));
		exit(-1);
	}

	free(regs8);
	free(regs16);
	modbus_close(ctx);
	modbus_free(ctx);

	return 0;
}
