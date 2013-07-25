lib-y+=pilot_wtk
pilot_wtk_SOURCES=pilot_display.c \
	pilot_input.c \
	pilot_window.c \
	pilot_canvas.c \
	pilot_buffer.c \
	pilot_widget.c \
	pilot_layout.c \
	pilot_theme.c \
	pilot_form.c \
	../weston_shared/os-compatibility.c

pilot_wtk-objs=$(pilot_wtk_SOURCES:%.c=%.o)

pilot_wtk_CFLAGS+=-I./include -I./weston_shared
ifeq ($(HAVE_KEYBOARD),y)
pilot_wtk_CFLAGS+=-DHAVE_KEYBOARD
endif
ifeq ($(HAVE_POINTER),y)
pilot_wtk_CFLAGS+=-DHAVE_POINTER
endif
pilot_wtk_CFLAGS+=-I$(src)/$(PLATFORM)
pilot_wtk_LDFLAGS+=-rdynamic
pilot_wtk_LIBRARY+=dl

ifeq ($(PLATFORM),wayland)
WAYLAND_PREFIX=$(if $(WLD),$(WLD),/usr)
WAYLAND_CFLAGS=-I$(WAYLAND_PREFIX)/include
WAYLAND_LDFLAGS=-L$(WAYLAND_PREFIX)/lib
WAYLAND_LIBRARY=wayland-client wayland-cursor
pilot_wtk_CFLAGS+=$(WAYLAND_CFLAGS)
pilot_wtk_LDFLAGS+=$(WAYLAND_LDFLAGS)
pilot_wtk_LIBRARY+=$(WAYLAND_LIBRARY)
endif

$(foreach s, $(pilot_wtk-objs), $(eval $(s:%.o=%)_CFLAGS+=$(pilot_wtk_CFLAGS)) )
