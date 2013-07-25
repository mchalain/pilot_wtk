#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <pilot_atk.h>
#include <pilot_wtk.h>
#include <pilot_utk.h>

#define SHM_FORMAT_SIZE(format)	_s_shm_format_size[format]
static int _s_shm_format_size[] = { 4, 4};

static void
_platform_buffer_destroy(struct pilot_buffer *buffer);
static int
_platform_buffer_create(struct pilot_buffer *buffer, struct pilot_widget *widget, int fd);
static void
_platform_buffer_paint_window(struct pilot_buffer *buffer, struct pilot_window *window);

struct pilot_buffer *
pilot_buffer_create(struct pilot_widget *parent,
				pilot_length_t width,
				pilot_length_t height,
				pilot_pixel_format_t format)
{
	struct pilot_buffer *buffer;
	int fd, pixel;
	void *data;

	buffer = calloc(1, sizeof *buffer);
	if (!buffer)
		return NULL;
	memset(buffer, 0, sizeof(*buffer));

	buffer->parent = parent;

	pixel = SHM_FORMAT_SIZE(format);
	buffer->format = format;
	buffer->width = width;
	buffer->height = height;
	buffer->stride = width * pixel;
	buffer->size =  height * buffer->stride;

	fd = os_create_anonymous_file(buffer->size);
	if (fd < 0) {
		LOG_ERROR("creating a buffer file for %d B failed: %m\n",
			buffer->size);
		return NULL;
	}

	data = mmap(NULL, buffer->size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (data == MAP_FAILED) {
		LOG_ERROR("mmap failed: %m\n");
		close(fd);
		return NULL;
	}
	buffer->shm_data = data;

	// paint the padding
	memset(buffer->shm_data, 0xff, buffer->size);

	_platform_buffer_create(buffer, parent, fd);
	close(fd);
	mutex_init(buffer->lock, NULL);
	cond_init(buffer->cond, NULL);

	return buffer;
}

void
pilot_buffer_destroy(struct pilot_buffer *buffer)
{
	mutex_destroy(buffer->lock);
	cond_destroy(buffer->cond);
	_platform_buffer_destroy(buffer);
	buffer->busy =0;
	free(buffer);
}

int
pilot_buffer_set_region(struct pilot_buffer *buffer, pilot_rect_t *rect)
{
	if (rect) {
		memcpy(&buffer->region,rect, sizeof buffer->region);
		buffer->regionning = 1;
	} else {
		buffer->regionning = 0;
	}
	return 0;
}

int
pilot_buffer_fill(struct pilot_buffer *buffer, pilot_color_t color)
{
	int ret;
	// paint the padding
	if (!buffer->regionning) {
		ret = colorset(buffer->shm_data, color, buffer->width * buffer->height);
		ret = ret?1:0;
	} else {
		int i;
		uint32_t height = buffer->region.y + buffer->region.h;
		pilot_color_t * shmem = ((pilot_color_t *)buffer->shm_data) + buffer->region.x;
		for (i = buffer->region.y; i < height; i ++, shmem += buffer->stride) {
			ret = colorset(shmem, color, buffer->region.w)? 1: 0;
			if (!ret) break;
		}
	}
	return ret;
}

void
pilot_buffer_paint_window(struct pilot_buffer *buffer, struct pilot_window *window)
{
	LOG_DEBUG("");
	if (! mutex_lock(buffer->lock)) {
		_platform_buffer_paint_window(buffer, window);
		buffer->busy=1;
		buffer->ready = 0;
		mutex_unlock(buffer->lock);
	}
}

int
pilot_buffer_lock(struct pilot_buffer *buffer, void **image)
{
	int ret;
	if (!(ret = mutex_lock(buffer->lock))) {
		while ( buffer->busy || buffer->ready) {
			LOG_DEBUG("ret %d",ret);
			ret = cond_wait(buffer->cond, buffer->lock);
			if (ret)
				break;
		}
		*image = (void *)buffer->shm_data;
	}
	return ret;
}

void 
pilot_buffer_unlock(struct pilot_buffer *buffer)
{
	buffer->ready = 1;
	mutex_unlock(buffer->lock);
}

void
pilot_buffer_release(struct pilot_buffer *buffer)
{
	if (!mutex_lock(buffer->lock)) {
		buffer->busy=0;
		cond_signal(buffer->cond);
		mutex_unlock(buffer->lock);
	}
}

#ifndef HAVE_INLINE
uint32_t
pilot_buffer_size(struct pilot_buffer *buffer)
{
	return buffer->size;
}

pilot_bool_t
pilot_buffer_busy(struct pilot_buffer *buffer)
{
	return buffer->busy;
}

pilot_bool_t
pilot_buffer_ready(struct pilot_buffer *buffer)
{
	return buffer->ready;
}

#endif

#include "platform_buffer.c"
