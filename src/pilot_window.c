#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pilot_utk.h>
#include <pilot_wtk.h>
#include <pilot_atk.h>

#include "pilot_wtk_internal.h"

static void
_pilot_window_destroy(struct pilot_window *thiz);
static int
_pilot_window_paint(struct pilot_window *thiz);

struct pilot_window *
pilot_window_create(struct pilot_display *display, char *name, struct pilot_rect rect)
{
	PILOT_CREATE_THIZ(pilot_window);

	thiz->type = EWidgetWindow;
	thiz->display = display;
	thiz->surface = pilot_surface_create(display, rect);
	memcpy(&thiz->drawingrect, &rect, sizeof(thiz->drawingrect));
	pilot_connect(display, synch, thiz, _pilot_window_paint);

	if (name) {
		thiz->name = malloc(strlen(name) + 1);
		strcpy(thiz->name, name);
	}

	thiz->action.destroy = _pilot_window_destroy;
	pilot_list_append(display->windows, thiz);

	return thiz;
}

static void
_pilot_window_destroy(struct pilot_window *thiz)
{
	pilot_list_remove(thiz->display->windows, thiz);
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
	ret = pilot_window_redraw(thiz);
	return ret;
}

static int
_pilot_window_paint(struct pilot_window *thiz)
{
	int ret = 0;
	ret = pilot_surface_paint(thiz->surface);
	if (thiz->force_redraw && thiz->surface->ready)
		pilot_window_redraw(thiz);
	return ret;
}

int
pilot_window_fullscreen(struct pilot_window *thiz)
{
	return 0;
}

int
pilot_window_redraw(struct pilot_window *thiz)
{
	int ret = -1;
	void *image;
	pilot_color_t color = 0xFFFFFF00;
	if (pilot_surface_lock(thiz->surface, &image) == 0)
	{
		if (image)
		{
			thiz->force_redraw = 0;

			struct pilot_rect rect = {
				.x = 0, .y = 0,
				.w = thiz->surface->width,
				.h = thiz->surface->height, };
			struct pilot_blit * video = pilot_blit_create(image, rect, 
										pilot_display_format(thiz->display));
			ret = pilot_blit_fill(video, color);
			if (thiz->layout)
				ret = pilot_widget_redraw(thiz->layout, video);
			pilot_blit_destroy(video);
		}
		pilot_surface_unlock(thiz->surface);
		pilot_surface_flip(thiz->surface);
	}
	return ret;
}

static int
_pilot_window_resize(void *widget, uint32_t width, uint32_t height)
{
	int ret = 0;
	return ret;
}

int
pilot_window_setfocus(struct pilot_window *thiz, struct pilot_widget *widget)
{
	int ret = -1;
	if(widget == NULL)
	{
		thiz->focus = pilot_widget_getfocus(thiz->layout);
		ret = 0;
	}
	else
	{
		struct pilot_widget *child = thiz->layout;
		// search widget inside the layout's childs
		thiz->focus = widget;
		ret = 0;
	}
	return ret;
}

