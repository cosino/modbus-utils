ifeq ($(V),s)
  Q = 
  DP_P = @true
  CC_P = @true
  LD_P = @true
  LB_P = @true
  RM_P = @true
else
  Q = @
  DP_P = @echo " DEPEND\t"
  CC_P = @echo " CC\t"
  LD_P = @echo " LD\t"
  LB_P = @echo " LIB\t"
  RM_P = @echo " REMOVE\t"
endif

#
# Functions
#

define inc_depend
-include $(SRCS_$(1):.c=.d)
endef

define do_lib
$(1) : $(SRCS_$(1):.c=.o)
	$(Q)$(CC) -shared $$^ -o $$@
	$(LB_P) $$^
endef

define do_gcc
$(1) : $(SRCS_$(1):.c=.o)
	$(Q)$(CC) $(foreach f,$(LDS_$(1)),-T $f) $(LDFLAGS) $(LDFLAGS_$(1)) $$^ -o $$@ $(LDLIBS) $(LDLIBS_$(1))
	$(LD_P) $$^
endef

define do_bin_clean
clean_$(1):
	$(Q)rm -rf $(1)
	$(RM_P) $(1)
	$(Q)rm -rf $(SRCS_$(1):.c=.o)
	$(RM_P) $(SRCS_$(1):.c=.o)
	$(Q)rm -rf $(SRCS_$(1):.c=.d)
	$(RM_P) $(SRCS_$(1):.c=.d)
endef

define do_install_bin
install_bin_$(1): $(1)
	install -d $(PREFIX)/bin
	install --target-directory=$(PREFIX)/bin/ $(1)
endef

#
# Targets
#

# All
all: $(LIBRARIES) $(TARGETS)
$(TARGETS): $(LIBRARIES)

%.d : %.c
	$(Q)$(CC) $(CCFLAGS) -fpic $(CPPFLAGS) \
		-MF $@ -MG -MM -MP -MT $(<:.c=.o) $<
	$(DP_P) $@

%.o : %.c
	$(Q)$(CC) -c $(CFLAGS) -fpic $(CPPFLAGS) $< -o $@
	$(CC_P) $@

$(foreach t,$(TARGETS) $(LIBRARIES),$(eval $(call inc_depend,$(t))))
$(foreach t,$(LIBRARIES),$(eval $(call do_lib,$(t))))
$(foreach t,$(TARGETS),$(eval $(call do_gcc,$(t))))

# Clean
clean: $(foreach t,$(TARGETS) $(LIBRARIES),clean_$(t) )

$(foreach t,$(TARGETS) $(LIBRARIES),$(eval $(call do_bin_clean,$(t))))

# Install
install: $(foreach t,$(TARGETS),install_bin_$(t) ) \
	install -d $(PREFIX)/var/run
	install -d $(PREFIX)/var/lock
$(foreach t,$(TARGETS),$(eval $(call do_install_bin,$(t))))
