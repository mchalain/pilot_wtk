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
};

inline struct  platform_display *
_platform_display(struct  pilot_display *display);
inline struct  platform_surface *
_platform_surface(struct  pilot_surface *surface);

#endif
