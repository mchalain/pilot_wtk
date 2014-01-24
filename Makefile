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
STATIC=
PTHREAD=y

ifdef PTHREAD
CFLAGS+=-DPTHREAD
LDFLAGS+=-pthread
LIBRARY+=pthread
endif

include $(src)/pilot_atk.mk
include $(src)/pilot_wtk.mk
include $(src)/pilot_utk.mk
include $(src)/test.mk

include ./scripts.mk

clean:
	$(RM) $(target-objs)
distclean: clean
	$(RM) $(lib-dynamic-target) $(lib-static-target)

