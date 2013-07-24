obj=obj
src=src
srctree=.

bin-y=
lib-y=

bin-ext=
slib-ext=a
dlib-ext=so

include ./config

CFLAGS=-g -DDEBUG
STATIC=1
DYNAMIC=1
TEST=1

include $(src)/pilot_wtk.mk
include $(src)/pilot_atk.mk
include $(src)/pilot_utk.mk
ifdef TEST
include $(src)/test.mk
endif

include ./scripts.mk

clean:
	$(RM) $(target-objs)
distclean: clean
	$(RM) $(lib-dynamic-target) $(lib-static-target)

