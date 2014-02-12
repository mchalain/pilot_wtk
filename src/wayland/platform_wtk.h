#ifndef __PLATFORM_WTK_H__
#define __PLATFORM_WTK_H__

#include <wayland-client.h>

struct  platform_display {
	struct wl_display *display;
	struct wl_registry *registry;
	struct wl_compositor *compositor;
	struct wl_shell *shell;
	struct wl_shm *shm;
	struct wl_seat *seat;
};

struct platform_surface {
	struct wl_surface *surface;
	struct wl_shell_surface *shell_surface;
	struct wl_callback *callback;
	struct wl_region *region;
};

struct platform_buffer {
	struct wl_buffer *buffer;
	int fd;
};

struct platform_input {
	struct wl_keyboard *keyboard;
	struct wl_pointer *pointer;
	struct wl_touch *touch;
	pilot_coord_t x, y;
};

inline struct  platform_display *
_platform_display(struct  pilot_display *display);
inline struct  platform_surface *
_platform_surface(struct  pilot_surface *surface);

struct pilot_window *
_platform_display_search_window(struct pilot_display *display, struct wl_surface *surface);

void
_platform_display_inputmanager(struct pilot_display *display, struct wl_registry *registry, uint32_t id);

#endif
