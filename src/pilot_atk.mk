lib-y+=pilot_atk
pilot_atk_SOURCES=pilot_application.c 
pilot_atk-objs=$(pilot_atk_SOURCES:%.c=%.o)

$(addprefix $(obj)/,$(pilot_atk-objs)):CFLAGS+=-I./include
