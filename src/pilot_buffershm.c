#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <pilot_atk.h>
#include <pilot_wtk.h>
#include <pilot_utk.h>

#include "pilot_wtk_internal.h"

static int
_platform_buffer_paint(struct pilot_buffer *thiz);
static void *
_platform_buffer_create(struct pilot_buffer *thiz, int fd);
static void
_platform_buffer_destroy(struct pilot_buffer *buffer);

static void
_pilot_buffershm_destroy(struct pilot_buffer *thiz);

struct pilot_buffer *
pilot_buffershm_create(struct pilot_surface *surface,
				int size)
{
	struct pilot_buffer *thiz;
	int fd;
	void *data;

	thiz = pilot_buffer_create(surface, size);

	fd = os_create_anonymous_file(size);
	if (fd < 0) {
		LOG_ERROR("creating a buffer file for %d B failed: %m\n",
			size);
		return NULL;
	}

	thiz->data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (thiz->data == MAP_FAILED) {
		LOG_ERROR("mmap failed: %m\n");
		close(fd);
		pilot_buffer_destroy(thiz);
		return NULL;
	}

	// paint the padding
	memset(thiz->data, 0xff, thiz->size);
	thiz->action.paint = _platform_buffer_paint;
	thiz->action.destroy = _pilot_buffershm_destroy;

	thiz->platform = _platform_buffer_create(thiz, fd);
	return thiz;
}

static void
_pilot_buffershm_destroy(struct pilot_buffer *thiz)
{
	_platform_buffer_destroy(thiz);
	_pilot_buffer_destroy(thiz);
}

#include "platform_buffer.c"
