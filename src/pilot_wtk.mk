lib-y+=pilot_wtk
pilot_wtk_SOURCES=pilot_display.c \
	pilot_input.c \
	pilot_window.c \
	pilot_canvas.c \
	pilot_buffer.c \
	pilot_widget.c \
	pilot_layout.c \
	pilot_theme.c \
	../weston_shared/os-compatibility.c

pilot_wtk-objs=$(pilot_wtk_SOURCES:%.c=%.o)

$(addprefix $(obj)/,$(pilot_wtk-objs)):CFLAGS+=-I./include -I./weston_shared
ifeq ($(HAVE_KEYBOARD),y)
$(addprefix $(obj)/,$(pilot_wtk-objs)):CFLAGS+=-DHAVE_KEYBOARD
endif
ifeq ($(HAVE_POINTER),y)
$(addprefix $(obj)/,$(pilot_wtk-objs)):CFLAGS+=-DHAVE_POINTER
endif
$(addprefix $(obj)/,$(pilot_wtk-objs)):CFLAGS+=-I$(src)/$(PLATFORM)

ifeq ($(PLATFORM),wayland)
WAYLAND_PREFIX=$(if $(WLD),$(WLD),/usr)
WAYLAND_CFLAGS=-I$(WAYLAND_PREFIX)/include
WAYLAND_LDFLAGS=-L$(WAYLAND_PREFIX)/lib
WAYLAND_LIBRARY=-lwayland-client -lwayland-cursor
$(addprefix $(obj)/,$(pilot_wtk-objs)):CFLAGS+=$(WAYLAND_CFLAGS)
$(addprefix $(obj)/,$(pilot_wtk-objs)):LDFLAGS+=$(WAYLAND_LDFLAGS)
$(addprefix $(obj)/,$(pilot_wtk-objs)):LIBRARY+=$(WAYLAND_LIBRARY)
endif
