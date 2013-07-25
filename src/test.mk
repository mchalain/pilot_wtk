test-libs=pilot_atk pilot_wtk pilot_utk

bin-y+=test0
test0_CFLAGS=-I./include
test0_LIBRARY=$(test-libs)
bin-y+=test1
test1_CFLAGS=-I./include
test1_LIBRARY=$(test-libs)
bin-y+=test1-2
test1-2_CFLAGS=-I./include
test1-2_LIBRARY=$(test-libs)
bin-y+=test3
test3-objs=test3.o test3.res.o
test3_CFLAGS=-I./include
test3.res_CFLAGS=-I./include
test3_LIBRARY=$(test-libs)
test3_LDFLAGS+=-rdynamic

