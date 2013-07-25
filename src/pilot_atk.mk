lib-y+=pilot_atk
pilot_atk_SOURCES=pilot_application.c 
pilot_atk-objs=$(pilot_atk_SOURCES:%.c=%.o)
pilot_atk_CFLAGS=-I./include
$(foreach s, $(pilot_atk-objs), $(eval $(s:%.o=%)_CFLAGS+=$(pilot_atk_CFLAGS)) )
