targets=test1

target=test1

CC=$(CROSS_COMPILE)gcc
LD=$(CROSS_COMPILE)gcc

WAYLAND_PREFIX=/usr/local
WAYLAND_CFLAGS=-I$(WAYLAND_PREFIX)/include
WAYLAND_LDFLAGS=-Wl,-rpath-link=$(WAYLAND_PREFIX)/lib -Wl,-rpath=$(WAYLAND_PREFIX)/lib -L$(WAYLAND_PREFIX)/lib -lwayland-client -lwayland-cursor

pilot_SOURCES=pilot_display.c pilot_window.c pilot_canvas.c pilot_buffer.c ../weston_shared/os-compatibility.c
$(target)_SOURCES=$(target).c

$(pilot_SOURCES:%.c=%.o) $($(target)_SOURCES:%.c=%.o):%.o:%.c
	$(CC) $(CFLAGS) $(WAYLAND_CFLAGS) -I../include -I../weston_shared -c -o $@ $<

$(target): $(pilot_SOURCES:%.c=%.o) $($(target)_SOURCES:%.c=%.o)
	$(LD) $(LDFLAGS) -o $@ $^  $(WAYLAND_LDFLAGS)
