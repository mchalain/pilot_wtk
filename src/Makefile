libname=pilotwtk
lib-static=lib$(libname).a
targets=test1 test1-2
include ../config

CC=$(CROSS_COMPILE)gcc
LD=$(CROSS_COMPILE)gcc
AR=$(CROSS_COMPILE)ar
RANLIB=$(CROSS_COMPILE)ranlib

CFLAGS=-g

WAYLAND_PREFIX=/usr/local
WAYLAND_CFLAGS=-I$(WAYLAND_PREFIX)/include
WAYLAND_LDFLAGS=-Wl,-rpath-link=$(WAYLAND_PREFIX)/lib -Wl,-rpath=$(WAYLAND_PREFIX)/lib -L$(WAYLAND_PREFIX)/lib
WAYLAND_LIBRARY=-lwayland-client -lwayland-cursor

pilot_SOURCES=pilot_display.c pilot_window.c pilot_canvas.c pilot_buffer.c pilot_widget.c pilot_layout.c pilot_theme.c ../weston_shared/os-compatibility.c
ifeq ($(HAVE_KEYBOARD),y)
CFLAGS+=-DHAVE_KEYBOARD
endif
ifeq ($(HAVE_POINTER),y)
CFLAGS+=-DHAVE_POINTER
endif

CFLAGS+=-I$(PLATFORM)

ifeq ($(PLATFORM),wayland)
CFLAGS+=$(WAYLAND_CFLAGS)
LDFLAGS+=$(WAYLAND_LDFLAGS)
LIBRARY+=$(WAYLAND_LIBRARY)
endif

$(targets)_SOURCES:=$(targets:%=%.c)

$(lib-static): $(pilot_SOURCES:%.c=%.o)
	$(AR) -cvq $@ $^
	$(RANLIB) $@

libpilot=$(lib-static)

$(targets): $($(targets)_SOURCES:%.c=%.o)
$(targets): $(libpilot)
	$(LD) $(LDFLAGS) -o $@ $@.o $(libpilot) $(LIBRARY)

$(pilot_SOURCES:%.c=%.o) $($(targets)_SOURCES:%.c=%.o):%.o:%.c
	$(CC) $(CFLAGS) -I../include -I../weston_shared -c -o $@ $<

clean:
	$(RM) -rf $(pilot_SOURCES:%.c=%.o) $($(target)_SOURCES:%.c=%.o) $(targets)
