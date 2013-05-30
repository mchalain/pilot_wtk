#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <pilot_wtk.h>

static void
_pilot_buffer_release(void *data, struct wl_buffer *buffer)
{
	struct pilot_buffer *mybuf = data;
	mybuf->busy = 0;
}

static const struct wl_buffer_listener _st_buffer_listener = {
	_pilot_buffer_release
};

#define SHM_FORMAT_SIZE(format)	_s_shm_format_size[format]
static int _s_shm_format_size[] = { 4, 4};
struct pilot_buffer *
pilot_buffer_create(struct pilot_display *display,
		  uint32_t width, uint32_t height, uint32_t format)
{
	struct pilot_buffer *buffer;
	struct wl_shm_pool *pool;
	int fd, size, stride;
	void *data;

	buffer = calloc(1, sizeof *buffer);
	if (!buffer)
		return NULL;
	memset(buffer, 0, sizeof(*buffer));

	stride = width * SHM_FORMAT_SIZE(format);
	size = stride * height;

	fd = os_create_anonymous_file(size);
	if (fd < 0) {
		fprintf(stderr, "creating a buffer file for %d B failed: %m\n",
			size);
		return NULL;
	}

	data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (data == MAP_FAILED) {
		fprintf(stderr, "mmap failed: %m\n");
		close(fd);
		return NULL;
	}
	buffer->shm_data = data;

	pool = wl_shm_create_pool(display->platform.shm, fd, size);
	buffer->buffer = wl_shm_pool_create_buffer(pool, 0,
						   width, height,
						   stride, format);
	wl_buffer_add_listener(buffer->buffer, &_st_buffer_listener, buffer);
	wl_shm_pool_destroy(pool);
	close(fd);

	// paint the padding
	memset(buffer->shm_data, 0xff, width * height * SHM_FORMAT_SIZE(format));

	return buffer;
}

void
pilot_buffer_destroy(struct pilot_buffer *buffer)
{
	wl_buffer_destroy(buffer->buffer);
	free(buffer);
}

