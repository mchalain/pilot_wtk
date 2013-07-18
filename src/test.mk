bin-y+=test0
bin-y+=test1
bin-y+=test1-2

$(bin-y):CFLAGS+=-I../include
$(bin-y):LDFLAGS+=-L/usr/local/lib
$(bin-y):LIBRARY+=-lwayland-client
test0-libs+=libpilot_atk$(slib-ext:%=.%) libpilot_wtk$(slib-ext:%=.%) libpilot_utk$(slib-ext:%=.%)
test1-libs+=libpilot_atk$(slib-ext:%=.%) libpilot_wtk$(slib-ext:%=.%) libpilot_utk$(slib-ext:%=.%)
test1-2-libs+=libpilot_atk$(slib-ext:%=.%) libpilot_wtk$(slib-ext:%=.%) libpilot_utk$(slib-ext:%=.%)
