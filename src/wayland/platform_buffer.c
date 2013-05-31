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

	pool = wl_shm_create_pool(widget->display->platform.shm, fd, buffer->size);
	buffer->platform.buffer = wl_shm_pool_create_buffer(pool, 0,
						   widget->width, widget->height,
						   widget->width * SHM_FORMAT_SIZE(widget->format),
						   widget->format);
	wl_buffer_add_listener(buffer->platform.buffer, &_st_buffer_listener, buffer);
	wl_shm_pool_destroy(pool);
	return 0;
}

static void
_platform_buffer_destroy(struct pilot_buffer *buffer)
{
	wl_buffer_destroy(buffer->platform.buffer);
}

static void
_platform_buffer_paint_window(struct pilot_buffer *buffer, struct pilot_window *window)
{
	struct wl_surface *surface = window->platform.surface;
	wl_surface_attach(surface, buffer->platform.buffer, buffer->parent->x, buffer->parent->y);
	wl_surface_damage(surface, 0, 0, window->fullwidth, window->fullheight);
	wl_surface_commit(surface);
}

