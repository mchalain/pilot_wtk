lib-y+=pilot_utk
pilot_utk_SOURCES=pilot_utils.c 
pilot_utk-objs=$(pilot_utk_SOURCES:%.c=%.o)
$(addprefix $(obj)/,$(pilot_wuk-objs)):CFLAGS+=-I./include
