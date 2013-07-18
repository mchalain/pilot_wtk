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

static int
_platform_buffer_create(struct pilot_buffer *buffer, struct pilot_widget *widget, int fd)
{
	struct wl_shm_pool *pool;
	struct platform_display *pl_display =
								widget->display->platform;
	struct platform_buffer *platform = malloc(sizeof(*platform));

	pool = wl_shm_create_pool(pl_display->shm, fd, buffer->size);
	platform->buffer = wl_shm_pool_create_buffer(pool, 0,
						   widget->width, widget->height,
						   widget->width * SHM_FORMAT_SIZE(widget->format),
						   widget->format);
	wl_buffer_add_listener(platform->buffer, &_st_buffer_listener, buffer);
	buffer->platform = platform;
	wl_shm_pool_destroy(pool);
	wl_display_roundtrip(pl_display->display);
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
_platform_buffer_paint_window(struct pilot_buffer *buffer, struct pilot_window *window)
{
	struct platform_buffer *platform = buffer->platform;
	struct platform_window *pl_window = window->platform;
	struct wl_surface *surface = pl_window->surface;
	wl_surface_attach(surface, platform->buffer, buffer->parent->x, buffer->parent->y);
}

