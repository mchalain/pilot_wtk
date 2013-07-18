obj=.
bin-ext=
slib-ext=a
dlib-ext=so

include ../config
quiet=quiet_
echo-cmd = $(if $($(quiet)cmd_$(1)), echo '  $($(quiet)cmd_$(1))';)
cmd = $(echo-cmd) $(cmd_$(1))

quiet_cmd_cc_o_c=CC $*
 cmd_cc_o_c=$(Q)$(CC) $(CFLAGS) -c -o $@ $<
quiet_cmd_ld_bin=LD $@
 cmd_ld_bin=$(Q)$(LD) $(LDFLAGS) -o $@ $^ $(LIBRARY)
quiet_cmd_ld_slib=LD $@
 cmd_ld_slib=$(RM) $@ && \
	$(AR) -cvq $@ $^ > /dev/null && \
	$(RANLIB) $@
quiet_cmd_ld_dlib=LD $@
 cmd_ld_dlib=$(Q)$(LD) -shared -Wl,-soname,$@ -o $@ $^ $(LIBRARY)


CC=$(CROSS_COMPILE)gcc
LD=$(CROSS_COMPILE)gcc
AR=$(CROSS_COMPILE)ar
RANLIB=$(CROSS_COMPILE)ranlib

CFLAGS=-g
STATIC=1
dynamic:DYNAMIC=1
test:TEST=1

include pilot_wtk.mk
include pilot_atk.mk
include pilot_utk.mk
ifdef TEST
include test.mk
endif

lib-static-target:=$(addprefix $(obj)/lib,$(addsuffix $(slib-ext:%=.%),$(lib-y)))
lib-dynamic-target:=$(addprefix $(obj)/lib,$(addsuffix $(dlib-ext:%=.%),$(lib-y)))
bin-target:=$(addprefix $(obj)/,$(addsuffix $(bin-ext:%=.%),$(bin-y)))

targets:=$(bin-target)
ifdef DYNAMIC
targets+=$(lib-dynamic-target)
endif
ifdef STATIC
targets+=$(lib-static-target)
endif

target-objs:=$(foreach t, $(lib-y) $(bin-y), $(if $($(t)-objs), $($(t)-objs), $(t).o))
all: $(targets)

test: $(bin-target)
dynamic: $(lib-dynamic-target)
static: $(lib-static-target)

$(obj)/%.o:%.c
	@$(call cmd,cc_o_c)

.SECONDEXPANSION:
$(lib-static-target): lib%$(slib-ext:%=.%): $$(if $$(%-objs), $$(%-objs), $(obj)/%.o)
	@$(call cmd,ld_slib)

$(lib-dynamic-target):CFLAGS+=-fPIC
$(lib-dynamic-target): lib%$(dlib-ext:%=.%): $$(if $$(%-objs), $$(%-objs), $(obj)/%.o)
	@$(call cmd,ld_dlib)

$(bin-target): %$(bin-ext:%=.%) : $$(if $$(%-objs), $$(%-objs), %.o) $(lib-dynamic-target) $$(%-libs)
	@$(call cmd,ld_bin)

clean:
	$(RM) $(target-objs)
distclean: clean
	$(RM) $(lib-dynamic-target) $(lib-static-target)
