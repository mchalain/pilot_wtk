#include "platform_wtk.h"
#include <sys/mman.h>

static void
_platform_fbuffer_release(void *data, struct wl_buffer *buffer)
{
	struct pilot_fbuffer *mybuf = data;
	LOG_DEBUG("buffer %p", buffer);
	mybuf->common.busy = 0;
}

static const struct wl_buffer_listener _st_fbuffer_listener = {
	_platform_fbuffer_release
};

static void *
_platform_fbuffer_create(struct pilot_fbuffer *fbuffer,
					struct pilot_display *display)
{
	struct pilot_surface *surface = (struct pilot_surface *)fbuffer;
	struct platform_buffer *platform;
	struct wl_shm_pool *pool;
	struct platform_display *pl_display = display->platform;
	int fd;
	void *data;

	platform = malloc(sizeof(*platform));
	memset((void *)platform, 0, sizeof *platform);
	
	fd = os_create_anonymous_file(surface->size);
	if (fd < 0) {
		LOG_ERROR("creating a buffer file for %d B failed: %m\n",
			surface->size);
		return NULL;
	}

	data = mmap(NULL, surface->size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (data == MAP_FAILED) {
		LOG_ERROR("mmap failed: %m\n");
		close(fd);
		return NULL;
	}
	// paint the padding
	memset(data, 0xff, surface->size);

	pool = wl_shm_create_pool(pl_display->shm, fd, surface->size);
	platform->buffer = wl_shm_pool_create_buffer(pool, 0,
						   surface->width, surface->height,
						   surface->stride,
						   surface->format);
	wl_buffer_add_listener(platform->buffer, &_st_fbuffer_listener, surface);
	fbuffer->platform = platform;
	wl_shm_pool_destroy(pool);
	wl_display_roundtrip(pl_display->display);
	close(fd);
	LOG_DEBUG("%p %d x %d for %p", fbuffer, surface->width, surface->height, display);
	return data;
}

static void
_platform_fbuffer_destroy(struct pilot_fbuffer *fbuffer)
{
	struct platform_buffer *platform = fbuffer->platform;
	wl_buffer_destroy(platform->buffer);
	free(platform);
}

static void
_platform_fbuffer_paint_window(void *surface,
					struct pilot_window *window)
{
	struct pilot_fbuffer *buffer = (struct pilot_fbuffer *)surface;
	struct platform_buffer *platform = buffer->platform;
	struct platform_window *pl_window = window->platform;
LOG_DEBUG("");
	wl_surface_attach(pl_window->surface, platform->buffer, 0, 0);
}
