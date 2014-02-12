#include <errno.h>
#include "platform_wtk.h"

static void *
_platform_buffer_create(struct pilot_buffer *buffer)
{
	PILOT_CREATE_THIZ(platform_buffer);
	buffer->data = mmap(NULL, buffer->size,
	                  PROT_WRITE, MAP_SHARED, _platform_surface_fb_fd(buffer->surface), 
	                  buffer->size * buffer->id);
	if (buffer->data == MAP_FAILED)
	{
		LOG_ERROR("mmap error: %s", strerror(errno));
		free(thiz);
		return NULL;
	}
	return (void *)thiz;
}

static void
_platform_buffer_destroy(struct pilot_buffer *buffer)
{
	struct platform_buffer *platform = buffer->platform;
	munmap(buffer->data, buffer->size);
	free(platform);
}

static int
_platform_buffer_paint(struct pilot_buffer *buffer)
{
	struct platform_buffer *platform = buffer->platform;
	struct platform_surface *surface = _platform_surface(buffer->surface);

	return 0;
}

