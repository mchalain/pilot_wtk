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

struct pilot_buffer *
pilot_buffer_create(struct pilot_surface *surface,
				int size)
{
	PILOT_CREATE_THIZ(pilot_buffer);

	mutex_init(thiz->lock, NULL);
	cond_init(thiz->cond, NULL);

	thiz->surface = surface;
	thiz->size = size;
	thiz->action.destroy = _pilot_buffer_destroy;
	return thiz;
}

void
_pilot_buffer_destroy(struct pilot_buffer *thiz)
{
	mutex_destroy(thiz->lock);
	cond_destroy(thiz->cond);
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

int
pilot_buffer_lock(struct pilot_buffer *thiz, void **shm)
{
	int ret = -1;
	if (!mutex_lock(thiz->lock))
	{
		while (thiz->busy || thiz->ready)
		{
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
	thiz->ready = 1;
	mutex_unlock(thiz->lock);
	return 0;
}
