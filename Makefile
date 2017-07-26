TARGETS = modbus-dump modbus-get modbus-set modbus-test

CFLAGS = -Wall -O2 -D_GNU_SOURCE $(shell pkg-config --cflags libmodbus)
LDLIBS = $(shell pkg-config --libs libmodbus)

SRCS_modbus-dump  = modbus-dump.c common.c
SRCS_modbus-get   = modbus-get.c common.c
SRCS_modbus-set   = modbus-set.c common.c
SRCS_modbus-test  = modbus-test.c common.c

include Makefile.inc
