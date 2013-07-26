#include <stdlib.h>
#include <pilot_wtk.h>

static void *
_platform_fbuffer_create(struct pilot_fbuffer *fbuffer,
					struct pilot_display *display);
static void
_platform_fbuffer_destroy(struct pilot_fbuffer *fbuffer);
static void
_platform_fbuffer_paint_window(void *surface,
					struct pilot_window *window);

struct pilot_fbuffer *
pilot_fbuffer_create(struct pilot_display *display,
					pilot_rect_t region,
					pilot_pixel_format_t format)
{
	struct pilot_fbuffer *fbuffer;

	fbuffer = malloc(sizeof *fbuffer);
	memset(fbuffer, 0, sizeof *fbuffer);
	pilot_surface_init(&fbuffer->common, region, format);
	fbuffer->common.action.destroy = _platform_fbuffer_destroy;
	fbuffer->common.action.paint_window = _platform_fbuffer_paint_window;
	fbuffer->display = display;
	fbuffer->common.data = _platform_fbuffer_create(fbuffer, display);
	return fbuffer;
}

#include "platform_fbuffer.c"
