lib-$(CONFIG_PILOT_UTK)+=pilot_utk
pilot_utk_SOURCES=pilot_utils.c 
pilot_utk_CFLAGS=$(CONFIG_PILOT_CFLAGS)
pilot_utk-objs=$(pilot_utk_SOURCES:%.c=%.o)
$(foreach s, $(pilot_utk-objs), $(eval $(s:%.o=%)_CFLAGS+=$(pilot_utk_CFLAGS)) )

