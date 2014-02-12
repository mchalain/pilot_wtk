#ifndef __PLATFORM_WTK_H__
#define __PLATFORM_WTK_H__

#include <linux/fb.h>

#include <wayland-client.h>

struct  platform_display {
};

struct platform_surface {
	int fd;
	struct fb_var_screeninfo vi;
	struct fb_fix_screeninfo fi;
};

struct platform_buffer {
};

struct platform_input {
};

inline struct  platform_display *
_platform_display(struct  pilot_display *display);
inline struct  platform_surface *
_platform_surface(struct  pilot_surface *surface);
inline int
_platform_surface_fb_fd(struct  pilot_surface *surface);
inline int
_platform_surface_fb_offset(struct  pilot_surface *surface, int id);
#endif
