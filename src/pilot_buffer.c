#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <pilot_atk.h>
#include <pilot_wtk.h>
#include <pilot_utk.h>

static void
_platform_buffer_destroy(struct pilot_buffer *buffer);
static int
_platform_buffer_create(struct pilot_buffer *buffer, struct pilot_widget *widget, int fd);
static void
_platform_buffer_paint_window(void *buffer, struct pilot_window *window);

struct pilot_buffer *
pilot_buffer_create(struct pilot_widget *parent,
				pilot_rect_t region,
				pilot_pixel_format_t format)
{
	struct pilot_buffer *buffer;
	int fd, pixel;
	void *data;

	buffer = calloc(1, sizeof *buffer);
	if (!buffer)
		return NULL;
	memset(buffer, 0, sizeof(*buffer));
	pilot_surface_init(&buffer->common, region, format);
	buffer->parent = parent;
	buffer->common.action.destroy = _platform_buffer_destroy;
	buffer->common.action.paint_window = _platform_buffer_paint_window;

	fd = os_create_anonymous_file(buffer->common.size);
	if (fd < 0) {
		LOG_ERROR("creating a buffer file for %d B failed: %m\n",
			buffer->common.size);
		return NULL;
	}

	data = mmap(NULL, buffer->common.size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (data == MAP_FAILED) {
		LOG_ERROR("mmap failed: %m\n");
		close(fd);
		return NULL;
	}
	buffer->common.data = data;

	// paint the padding
	memset(buffer->common.data, 0xff, buffer->common.size);

	_platform_buffer_create(buffer, parent, fd);
	close(fd);

	return buffer;
}

int
pilot_buffer_set_region(struct pilot_buffer *buffer, pilot_rect_t *rect)
{
	if (rect) {
		memcpy(&buffer->region,rect, sizeof buffer->region);
		buffer->common.regionning = 1;
	} else {
		buffer->common.regionning = 0;
	}
	return 0;
}

int
pilot_buffer_fill(struct pilot_buffer *buffer, pilot_color_t color)
{
	int ret;
	// paint the padding
	if (!buffer->common.regionning) {
		ret = colorset(buffer->common.data, color, buffer->common.width * buffer->common.height);
		ret = ret?1:0;
	} else {
		int i;
		uint32_t height = buffer->region.y + buffer->region.h;
		pilot_color_t * shmem = ((pilot_color_t *)buffer->common.data) + buffer->region.x;
		for (i = buffer->region.y; i < height; i ++, shmem += buffer->common.stride) {
			ret = colorset(shmem, color, buffer->region.w)? 1: 0;
			if (!ret) break;
		}
	}
	return ret;
}

#include "platform_buffer.c"
