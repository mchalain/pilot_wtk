#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <pilot_atk.h>
#include <pilot_wtk.h>
#include <pilot_utk.h>

#include "pilot_wtk_internal.h"

void
_pilot_buffer_destroy(struct pilot_buffer *thiz);

static void *
_platform_buffer_create(struct pilot_buffer *buffer, struct pilot_surface *surface);
static void
_platform_buffer_destroy(struct pilot_buffer *buffer);
static int
_platform_buffer_paint(struct pilot_buffer *buffer);

struct pilot_buffer *
pilot_buffer_create(struct pilot_surface *surface,
				int size)
{
	PILOT_CREATE_THIZ(pilot_buffer);

	mutex_init(thiz->lock, NULL);
	cond_init(thiz->cond, NULL);

	thiz->surface = surface;
	thiz->size = size;
	thiz->action.paint = _platform_buffer_paint;
	thiz->action.destroy = _pilot_buffer_destroy;

	thiz->platform = _platform_buffer_create(thiz, surface);
	if (!thiz)
	{
		mutex_destroy(thiz->lock);
		cond_destroy(thiz->cond);
		free(thiz);
		thiz = NULL;
	}
	return thiz;
}

void
_pilot_buffer_destroy(struct pilot_buffer *thiz)
{
	mutex_destroy(thiz->lock);
	cond_destroy(thiz->cond);
	_platform_buffer_destroy(thiz);
	free(thiz);
}

void
pilot_buffer_destroy(struct pilot_buffer *thiz)
{
	if (thiz->action.destroy)
		thiz->action.destroy(thiz);
}

int
pilot_buffer_paint(struct pilot_buffer *thiz)
{
	int ret = -1;
	if (!mutex_lock(thiz->lock))
	{
		if (thiz->action.paint)
			ret = thiz->action.paint(thiz);
		else
			ret = 0;
		thiz->busy = 1;
		thiz->ready = 0;
		mutex_unlock(thiz->lock);
	}
	return ret;
}

void
pilot_buffer_busy(struct pilot_buffer *thiz, int busy)
{
	if (!mutex_lock(thiz->lock))
	{
		thiz->busy = busy;
		cond_signal(thiz->cond);
		mutex_unlock(thiz->lock);
	}
}

int
pilot_buffer_lock(struct pilot_buffer *thiz, void **shm)
{
	int ret = -1;
	if (!mutex_lock(thiz->lock))
	{
		LOG_DEBUG("");
		ret = 0;
		while (thiz->busy || thiz->ready)
		{
			// becarefull cond_wait HAVE NOT TO block the main loop on select
			ret = cond_wait(thiz->cond, thiz->lock);
			if (ret)
			{
				mutex_unlock(thiz->lock);
			}
		}
		if (!ret && shm)
			*shm = thiz->data;
	}
	return ret;
}

int
pilot_buffer_unlock(struct pilot_buffer *thiz)
{
	LOG_DEBUG("");
	thiz->ready = 1;
	mutex_unlock(thiz->lock);
	return 0;
}

#include "platform_buffer.c"
