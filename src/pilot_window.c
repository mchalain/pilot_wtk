#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pilot_wtk.h>
#include <pilot_utk.h>

#include "pilot_wtk_internal.h"

static void
_pilot_window_destroy(struct pilot_window *thiz);
static int
_pilot_window_redraw(struct pilot_window *thiz);

struct pilot_window *
pilot_window_create(struct pilot_display *display, char *name, struct pilot_rect rect)
{
	struct pilot_window *thiz;
	thiz = calloc(1, sizeof(*thiz));
	if (!thiz)
		return NULL;
	memset(thiz, 0, sizeof(*thiz));

	thiz->display = display;
	thiz->surface = pilot_surface_create(display, rect);

	if (name) {
		thiz->name = malloc(strlen(name) + 1);
		strcpy(thiz->name, name);
	}

	memcpy(&thiz->drawingrect, &rect, sizeof(thiz->drawingrect));
	thiz->action.destroy = _pilot_window_destroy;

	return thiz;
}

static void
_pilot_window_destroy(struct pilot_window *thiz)
{
	if (thiz->surface) {
		pilot_surface_destroy(thiz->surface);
	}
	if (thiz->name)
		free(thiz->name);
	free(thiz);
}

void
pilot_window_destroy(struct pilot_window *thiz)
{
	if (thiz->action.destroy)
		thiz->action.destroy(thiz);
}

int
pilot_window_show(struct pilot_window *thiz)
{
	int ret = 0;
	_pilot_window_redraw(thiz);
	ret = pilot_surface_paint(thiz->surface);
	return ret;
}


int
pilot_window_fullscreen(struct pilot_window *thiz)
{
	return 0;
}

static int
_pilot_window_redraw(struct pilot_window *thiz)
{
	int ret = -1;
	void *image;
	pilot_color_t color = 0xFFFFFF00;
	pilot_surface_lock(thiz->surface, &image);
	if (image)
	{
		struct pilot_rect rect = {
			.x = 0, .y = 0,
			.w = thiz->surface->width,
			.h = thiz->surface->height, };
		struct pilot_memory * video = pilot_memory_create(image, rect);
		ret = pilot_memory_fill(video, color);

		pilot_memory_destroy(video);
	}
	pilot_surface_unlock(thiz->surface);
	pilot_surface_flip(thiz->surface);
	return ret;
}

static int
_pilot_window_resize(void *widget, uint32_t width, uint32_t height)
{
	int ret = 0;
	return ret;
}

