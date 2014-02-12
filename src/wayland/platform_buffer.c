#include "platform_wtk.h"

static void
_platform_buffer_release(void *data, struct wl_buffer *buffer)
{
	struct pilot_buffer *mybuf = data;
	pilot_buffer_busy(mybuf, 0);
}

static const struct wl_buffer_listener _st_buffer_listener = {
	_platform_buffer_release
};

static void *
_platform_buffer_create(struct pilot_buffer *buffer, struct pilot_surface *surface)
{
	struct wl_shm_pool *pool;
	struct platform_display *display = NULL;
	PILOT_CREATE_THIZ(platform_buffer);

	thiz->fd = os_create_anonymous_file(buffer->size);
	if (thiz->fd < 0) {
		LOG_ERROR("creating a buffer file for %llu B failed: %m\n",
			buffer->size);
		return NULL;
	}

	buffer->data = mmap(NULL, buffer->size, PROT_READ | PROT_WRITE, MAP_SHARED, thiz->fd, 0);
	if (buffer->data == MAP_FAILED) {
		LOG_ERROR("mmap failed: %m\n");
		close(thiz->fd);
		free(thiz);
		return NULL;
	}
	// paint the padding
	memset(buffer->data, 0xff, buffer->size);

	display = _platform_display(buffer->surface->display);

	pool = wl_shm_create_pool(display->shm, thiz->fd, buffer->size);
	
	thiz->buffer = wl_shm_pool_create_buffer(pool, 0,
								buffer->surface->width, buffer->surface->height,
								buffer->surface->stride, buffer->surface->format);
		LOG_DEBUG("");
	wl_buffer_add_listener(thiz->buffer, &_st_buffer_listener, buffer);

	wl_shm_pool_destroy(pool);

	wl_display_roundtrip(display->display);
	return (void *)thiz;
}

static void
_platform_buffer_destroy(struct pilot_buffer *buffer)
{
	struct platform_buffer *platform = buffer->platform;
	close(platform->fd);
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

