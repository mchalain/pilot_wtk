#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <pilot_wtk.h>

#define SHM_FORMAT_SIZE(format)	_s_shm_format_size[format]
static int _s_shm_format_size[] = { 4, 4};

static void
_platform_buffer_destroy(struct pilot_buffer *buffer);
static int
_platform_buffer_create(struct pilot_buffer *buffer, struct pilot_widget *widget, int fd);
static void
_platform_buffer_paint_window(struct pilot_buffer *buffer, struct pilot_window *window);

struct pilot_buffer *
pilot_buffer_create(struct pilot_widget *parent)
{
	struct pilot_buffer *buffer;
	int fd, stride, pixel;
	void *data;

	buffer = calloc(1, sizeof *buffer);
	if (!buffer)
		return NULL;
	memset(buffer, 0, sizeof(*buffer));

	buffer->parent = parent;

	pixel = SHM_FORMAT_SIZE(parent->format);
	buffer->size = parent->width * parent->height * pixel;

	fd = os_create_anonymous_file(buffer->size);
	if (fd < 0) {
		fprintf(stderr, "creating a buffer file for %d B failed: %m\n",
			buffer->size);
		return NULL;
	}

	data = mmap(NULL, buffer->size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (data == MAP_FAILED) {
		fprintf(stderr, "mmap failed: %m\n");
		close(fd);
		return NULL;
	}
	buffer->shm_data = data;

	_platform_buffer_create(buffer, parent, fd);
	close(fd);

	// paint the padding
	memset(buffer->shm_data, 0xff, buffer->size);

	return buffer;
}

void
pilot_buffer_destroy(struct pilot_buffer *buffer)
{
	_platform_buffer_destroy(buffer);
	free(buffer);
}

void
pilot_buffer_paint_window(struct pilot_buffer *buffer, struct pilot_window *window)
{
	_platform_buffer_paint_window(buffer, window);
	buffer->busy = 1;
}

#ifndef HAVE_INLINE
uint32_t
pilot_buffer_size(struct pilot_buffer *buffer)
{
	return buffer->size;
}

void
pilot_buffer_release(struct pilot_buffer *buffer)
{
	buffer->busy = 0;
}

pilot_mutex_t
pilot_buffer_busy(struct pilot_buffer *buffer)
{
	return buffer->busy;
}
#endif

#include "platform_buffer.c"
