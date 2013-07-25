#ifndef __PLATFORM_WTK_H__
#define __PLATFORM_WTK_H__

#include <wayland-client.h>

struct pilot_window *
_platform_display_search_window(struct pilot_display *display, struct wl_surface *surface);

struct  platform_display {
	struct wl_display *display;
	struct wl_registry *registry;
	struct wl_compositor *compositor;
	struct wl_shell *shell;
	struct wl_shm *shm;
	struct wl_seat *seat;
};

struct platform_window {
	struct wl_surface *surface;
	struct wl_shell_surface *shell_surface;
	struct wl_callback *callback;
	struct wl_region *region;
};

struct platform_buffer {
	struct wl_buffer *buffer;
};

struct platform_cursor {
	struct wl_cursor *cursor;
	struct wl_surface *surface;
	struct wl_cursor_theme *theme;
};

struct platform_eglcanvas {
	struct wl_egl_window *native;
};

struct platform_input {
	struct wl_seat *seat;
	struct wl_pointer *pointer;
	struct wl_keyboard *keyboard;
};
#endif
