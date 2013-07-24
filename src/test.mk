bin-y+=test0
bin-y+=test1
bin-y+=test1-2
test0-objs=test0.o
test1-objs=test1.o
test1-2-objs=test1-2.o
test-objs=$(addprefix $(obj)/,$(test0-objs) $(test1-objs) $(test1-2-objs))

$(test-objs):CFLAGS+=-I./include
$(addprefix $(obj)/,$(bin-y)):LDFLAGS+=-L/usr/local/lib
$(addprefix $(obj)/,$(bin-y)):LIBRARY+=-lwayland-client
test-libs=libpilot_atk libpilot_wtk libpilot_utk
ifdef STATIC
test-libs:=$(addsuffix $(slib-ext:%=.%), $(test-libs))
else
test-libs:=$(addsuffix $(dlib-ext:%=.%), $(test-libs))
endif
test-libs:=$(addprefix $(obj)/, $(test-libs))
test0-libs:=$(test-libs)
test1-libs:=$(test-libs)
test1-2-libs:=$(test-libs)
