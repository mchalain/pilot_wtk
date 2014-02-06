#include "platform_wtk.h"

static void
_platform_buffer_release(void *data, struct wl_buffer *buffer)
{
	struct pilot_buffer *mybuf = data;
	mybuf->busy = 0;
}

static const struct wl_buffer_listener _st_buffer_listener = {
	_platform_buffer_release
};

static void *
_platform_buffer_create(struct pilot_buffer *buffer, int fd)
{
	struct wl_shm_pool *pool;
	struct platform_display *display = NULL;
	PILOT_CREATE_THIZ(platform_buffer);

	display = _platform_display(buffer->surface->display);

	pool = wl_shm_create_pool(display->shm, fd, buffer->size);
	
	thiz->buffer = wl_shm_pool_create_buffer(pool, 0,
								buffer->surface->width, buffer->surface->height,
								buffer->surface->stride, buffer->surface->format);
	wl_buffer_add_listener(thiz->buffer, &_st_buffer_listener, buffer);

	wl_shm_pool_destroy(pool);

	wl_display_roundtrip(display->display);
	return (void *)thiz;
}

static void
_platform_buffer_destroy(struct pilot_buffer *buffer)
{
	struct platform_buffer *platform = buffer->platform;
	wl_buffer_destroy(platform->buffer);
	free(platform);
}

static int
_platform_buffer_paint(struct pilot_buffer *buffer)
{
	struct platform_buffer *platform = buffer->platform;
	struct platform_surface *surface = _platform_surface(buffer->surface);

	if (surface->surface && platform->buffer)
		wl_surface_attach(surface->surface, platform->buffer, 0, 0);
}

