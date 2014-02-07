#ifndef __PILOT_WTK_INTERNAL_H__
#define __PILOT_WTK_INTERNAL_H__

struct pilot_surface {
	struct pilot_display *display;
	struct pilot_buffer *buffers[2];
	struct pilot_buffer *onscreenbuffer;
	struct pilot_buffer *offscreenbuffer;
	pilot_bool_t offscreenbufferid:2;
	pilot_bool_t ready:1;
	pilot_bool_t regionning:1;
	uint32_t size;
	uint32_t stride;
	pilot_length_t width;
	pilot_length_t height;
	pilot_pixel_format_t format;
	_pilot_mutex(lock);
	_pilot_cond(cond);
	struct {
		void (*destroy)(struct pilot_surface *surface);
	} action;
	void *platform;
};

struct pilot_buffer {
	struct pilot_surface *surface;
	pilot_rect_t region;
	unsigned long long size;
	void *data;
	pilot_bool_t ready:1;
	pilot_bool_t busy:1;
	_pilot_mutex(lock);
	_pilot_cond(cond);
	struct {
		void (*destroy)(struct pilot_buffer *surface);
		int (*paint)(struct pilot_buffer *surface);
	} action;
	void *platform;
};

int
pilot_widget_redraw(struct pilot_widget *thiz);

/**
 * pilot_surface
 * **/
struct pilot_surface *
pilot_surface_create(struct pilot_display *display, pilot_rect_t rect);
void
pilot_surface_destroy(struct pilot_surface *thiz);
int
pilot_surface_paint(struct pilot_surface *thiz);
int
pilot_surface_lock(struct pilot_surface *thiz, void **image);
int
pilot_surface_unlock(struct pilot_surface *thiz);
int
pilot_surface_flip(struct pilot_surface *thiz);
/**
 * pilot_buffer
 * **/
struct pilot_buffer *
pilot_buffer_create(struct pilot_surface *surface,
				int size);
struct pilot_buffer *
pilot_buffershm_create(struct pilot_surface *surface,
				int size);
void
pilot_buffer_destroy(struct pilot_buffer *thiz);
#endif
