targets=test1
include ../config
target=test1

CC=$(CROSS_COMPILE)gcc
LD=$(CROSS_COMPILE)gcc


CFLAGS=-g

WAYLAND_PREFIX=/usr/local
WAYLAND_CFLAGS=-I$(WAYLAND_PREFIX)/include
WAYLAND_LDFLAGS=-Wl,-rpath-link=$(WAYLAND_PREFIX)/lib -Wl,-rpath=$(WAYLAND_PREFIX)/lib -L$(WAYLAND_PREFIX)/lib -lwayland-client -lwayland-cursor

pilot_SOURCES=pilot_display.c pilot_window.c pilot_canvas.c pilot_buffer.c pilot_widget.c pilot_layout.c pilot_theme.c ../weston_shared/os-compatibility.c
ifeq ($(HAVE_KEYBOARD),y)
CFLAGS+=-DHAVE_KEYBOARD
endif
ifeq ($(HAVE_POINTER),y)
CFLAGS+=-DHAVE_POINTER
endif

CFLAGS+=-I$(PLATFORM)

$(target)_SOURCES=$(target).c

$(pilot_SOURCES:%.c=%.o) $($(target)_SOURCES:%.c=%.o):%.o:%.c
	$(CC) $(CFLAGS) $(WAYLAND_CFLAGS) -I../include -I../weston_shared -c -o $@ $<

$(target): $(pilot_SOURCES:%.c=%.o) $($(target)_SOURCES:%.c=%.o)
	$(LD) $(LDFLAGS) -o $@ $^  $(WAYLAND_LDFLAGS)

clean:
	$(RM) -rf $(pilot_SOURCES:%.c=%.o) $($(target)_SOURCES:%.c=%.o) $(targets)
