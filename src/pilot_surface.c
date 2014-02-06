#include <stdlib.h>
#include <string.h>
#include <pilot_atk.h>
#include <pilot_wtk.h>

#include "pilot_wtk_internal.h"

#define SHM_FORMAT_SIZE(format)	_s_shm_format_size[format]
static int _s_shm_format_size[] = { sizeof(pilot_color_t), sizeof(pilot_color_t)};

static void *
_platform_surface_create(struct pilot_surface *surface, struct pilot_display *display);
static void
_platform_surface_destroy(struct pilot_surface *surface);
static int
_platform_surface_flush(struct pilot_surface *surface);

struct pilot_surface *
pilot_surface_create(struct pilot_display *display,
					pilot_rect_t region)
{
	PILOT_CREATE_THIZ(pilot_surface);
	int pixel;

	thiz->display = display;
	thiz->width = region.w;
	thiz->height = region.h;
	thiz->format = pilot_display_format(display);
	pixel = SHM_FORMAT_SIZE(thiz->format);
	thiz->stride = thiz->width * pixel;
	thiz->size =  thiz->height * thiz->stride;
	thiz->platform = _platform_surface_create(thiz, display);
	thiz->offscreenbufferid = 0;
	thiz->buffers[0] = pilot_buffershm_create(thiz, thiz->size);
	thiz->buffers[1] = pilot_buffershm_create(thiz, thiz->size);
	pilot_connect(display, synch, thiz, pilot_surface_paint);
	mutex_init(thiz->lock, NULL);
	cond_init(thiz->cond, NULL);

	return thiz;
}

void
pilot_surface_destroy(struct pilot_surface *thiz)
{
	mutex_destroy(thiz->lock);
	cond_destroy(thiz->cond);

	if (thiz->action.destroy)
		thiz->action.destroy(thiz);
	_platform_surface_destroy(thiz);
	free(thiz);
}

static int
_pilot_surface_flush(struct pilot_surface *thiz)
{
	return _platform_surface_flush(thiz);
}

int
pilot_surface_paint(struct pilot_surface *thiz)
{
	int ret = -1;
	if (!mutex_lock(thiz->lock))
	{
		if (thiz->onscreenbuffer && thiz->onscreenbuffer->ready)
		{
			ret = pilot_buffer_paint(thiz->onscreenbuffer);
			_pilot_surface_flush(thiz);
			thiz->onscreenbuffer = NULL;
		}
		cond_signal(thiz->cond);
		mutex_unlock(thiz->lock);
	}
	return ret;
}

int
pilot_surface_lock(struct pilot_surface *thiz, void **image)
{
	int ret = -1;
	*image = NULL;
	if (!thiz->offscreenbuffer)
	{
		thiz->offscreenbuffer = thiz->buffers[thiz->offscreenbufferid];
		thiz->offscreenbufferid = (thiz->offscreenbufferid + 1) & 0x1;
	}
	if (thiz->offscreenbuffer)
	{
		ret = pilot_buffer_lock(thiz->offscreenbuffer, image);
	}
	return ret;
}

int 
pilot_surface_unlock(struct pilot_surface *thiz)
{
	int ret = -1;
	if (thiz->offscreenbuffer)
	{
		ret = pilot_buffer_unlock(thiz->offscreenbuffer);
	}
	return ret;
}

int
pilot_surface_flip(struct pilot_surface *thiz)
{
	int ret = -1;
	if (!mutex_lock(thiz->lock))
	{
		ret = 0;
		while (thiz->onscreenbuffer)
		{
			ret = cond_wait(thiz->cond, thiz->lock);
		}
		if (!ret)
		{
			thiz->onscreenbuffer = thiz->offscreenbuffer;
			thiz->offscreenbuffer = NULL;
			pilot_display_synch(thiz->display);
		}
		mutex_unlock(thiz->lock);
	}
	return ret;
}

#include "platform_surface.c"
