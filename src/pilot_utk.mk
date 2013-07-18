lib-y+=pilot_utk

pilot_utk_SOURCES=pilot_utils.c 
pilot_utk-objs=$(addprefix $(obj)/,$(pilot_utk_SOURCES:%.c=%.o))
$(pilot_atk-objs):CFLAGS+=-I../include
