TARGETS = modbus-dump modbus-get modbus-set
COMMON_SRCS = common.c

CFLAGS := -Wall -O2 -D_GNU_SOURCE $(shell pkg-config --cflags libmodbus)
LDLIBS := $(shell pkg-config --libs libmodbus)

# Everything after this is generic

all: $(TARGETS)

.depend depend dep :
	$(CC) $(CFLAGS) -M $(COMMON_SRCS) $(TARGETS:=.c) > .depend

ifeq ($(wildcard .depend),.depend)
include .depend
endif

$(COMMON_SRCS:.c=.o) : .depend

define TARGET_template
$(1): $(1).o $(COMMON_SRCS:.c=.o)
endef
$(foreach target,$(TARGETS),$(eval $(call TARGET_template,$(target))))

clean:
	rm -rf $(TARGETS) $(TARGETS:=.o) $(COMMON_SRCS:.c=.o) .depend

.PHONY: all clean .depend depend dep
