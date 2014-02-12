#include "platform_wtk.h"

#define fb_width(fb)  ((fb)->vi.xres)
#define fb_height(fb)  ((fb)->vi.yres)
#define fb_bpp(fb)     ((fb)->vi.bits_per_pixel>>3)
#define fb_size(fb)    ((fb)->vi.xres * (fb)->vi.yres * fb_bpp(fb))
#define fb_stride(fb) ((fb)->fi.line_length)

static void *
_platform_surface_create(struct pilot_surface *surface, struct pilot_display *display)
{
	PILOT_CREATE_THIZ(platform_surface);

	/* Probe the device for screen information. */
	if (ioctl(display->connector->fd, FBIOGET_FSCREENINFO, &thiz->fi) < 0 ||
		ioctl(display->connector->fd, FBIOGET_VSCREENINFO, &thiz->vi) < 0)
	{
		free(thiz);
		return NULL;
	}
	thiz->fd = display->connector->fd;

	if (thiz->vi.red.length == thiz->vi.blue.length &&
		thiz->vi.red.length == thiz->vi.green.length &&
		thiz->vi.red.length == 8)
	{
		if ((thiz->vi.transp.offset >= thiz->vi.red.offset ||
				thiz->vi.transp.length == 0) &&
				thiz->vi.red.offset >= thiz->vi.green.offset &&
				thiz->vi.green.offset >= thiz->vi.blue.offset)
			surface->format = PILOT_DISPLAY_ARGB8888;
		else if (thiz->vi.red.offset >= thiz->vi.green.offset &&
				thiz->vi.green.offset >= thiz->vi.blue.offset &&
				thiz->vi.blue.offset >= thiz->vi.transp.offset)
			surface->format = PILOT_DISPLAY_XRGB8888;
	}

	surface->width = fb_width(thiz);
	surface->height = fb_height(thiz);
	surface->stride = fb_stride(thiz);
	surface->size =  fb_size(thiz);

	return (void *)thiz;
}

static void
_platform_surface_destroy(struct pilot_surface *surface)
{
	struct platform_surface *platform = surface->platform;
	free(platform);
}

static int
_platform_surface_flush(struct pilot_surface *surface)
{
	struct platform_surface *platform = surface->platform;
	platform->vi.yoffset = surface->size * surface->onscreenbuffer->id;
	if (ioctl(platform->fd, FBIOPAN_DISPLAY, &platform->vi) < 0)
		return -1;
	return 0;
}

inline struct  platform_surface *
_platform_surface(struct  pilot_surface *surface)
{
	return surface->platform;
}

inline int
_platform_surface_fb_fd(struct  pilot_surface *surface)
{
	return _platform_surface(surface)->fd;
}

inline int
_platform_surface_fb_offset(struct  pilot_surface *surface, int id)
{
	return fb_stride(_platform_surface(surface)) * fb_height(_platform_surface(surface)) * id;
}

/**************************************************************************************/

