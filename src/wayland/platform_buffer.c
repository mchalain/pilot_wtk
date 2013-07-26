#include "platform_wtk.h"

static void
_platform_buffer_release(void *data, struct wl_buffer *buffer)
{
	struct pilot_buffer *mybuf = data;
	LOG_DEBUG("");
	mybuf->common.busy = 0;
}

static const struct wl_buffer_listener _st_buffer_listener = {
	_platform_buffer_release
};

static int
_platform_buffer_create(struct pilot_buffer *buffer, struct pilot_widget *widget, int fd)
{
	struct wl_shm_pool *pool;
	struct pilot_surface *surface = (struct pilot_surface *)buffer;
	struct platform_display *pl_display =
								widget->display->platform;
	struct platform_buffer *platform = malloc(sizeof(*platform));

	pool = wl_shm_create_pool(pl_display->shm, fd, surface->size);
	platform->buffer = wl_shm_pool_create_buffer(pool, 0,
						   surface->width, surface->height,
						   surface->stride,
						   surface->format);
	wl_buffer_add_listener(platform->buffer, &_st_buffer_listener, buffer);
	buffer->platform = platform;
	wl_shm_pool_destroy(pool);
	wl_display_roundtrip(pl_display->display);
	LOG_DEBUG("%p %d x %d for %p", buffer, surface->width, surface->height, widget);
	return 0;
}

static void
_platform_buffer_destroy(struct pilot_buffer *buffer)
{
	struct platform_buffer *platform = buffer->platform;
	wl_buffer_destroy(platform->buffer);
	free(platform);
}

static void
_platform_buffer_paint_window(void *surface,
					struct pilot_window *window)
{
	struct pilot_buffer *buffer = (struct pilot_buffer *)surface;
	struct platform_buffer *platform = buffer->platform;
	struct platform_window *pl_window = window->platform;

	LOG_DEBUG(" at %d x %d", buffer->parent->region.x, buffer->parent->region.y);
	//wl_surface_attach(pl_window->surface, platform->buffer, 0, 0);
	wl_surface_attach(pl_window->surface, platform->buffer, buffer->parent->region.x, buffer->parent->region.y);
}

