#ifndef __PILOT_WTK_H__
#define __PILOT_WTK_H__

#include <stdint.h>
#include "pilot_types.h"
#include "pilot_log.h"
#include "pilot_signal.h"

#define MAXBUFFERS 2
#define MAXWIDGETS 4

struct pilot_application;
struct pilot_window;
struct pilot_eglcanvas;
struct pilot_canvas;
struct pilot_cursor;
struct pilot_theme;


#ifndef _pilot_list
#define _pilot_list(type, name) struct _pilot_list_##type {\
			struct type *item; \
			struct _pilot_list_##type *next; \
		} name
#define pilot_list_append(list, entry) do { typeof (list) *it = &list; \
			while (it->next) it = it->next; \
			it->item = entry; \
			it->next = malloc(sizeof(typeof (list))); \
			it = it->next; \
			memset(it, 0, sizeof(typeof (list))); \
		} while(0)
#define pilot_list_foreach(list, func, data) do { typeof (list) *it = &list; \
			while (it->next) { \
				func(data, it->item); \
				it = it->next; \
			} \
		} while(0)
#define pilot_list_destroy(list) do { typeof (list) *it = &list; \
			it = it->next; \
			while (it->next) { \
				typeof (list) *tmp = it->next; \
				free(it); \
				it = tmp; \
			} \
			free(it); \
		} while(0)
#endif

typedef enum
{
	PILOT_DISPLAY_ARGB8888,
	PILOT_DISPLAY_XRGB8888
} pilot_pixel_format_t;

struct pilot_widget {
	struct pilot_display *display;
	struct pilot_window *window;
	struct pilot_widget *parent;
	_pilot_signal(pilot_widget, clicked, pilot_key_t key);
	_pilot_signal(pilot_widget, scrolled, pilot_coord_t x, pilot_coord_t y);
	_pilot_signal(pilot_widget, keyPressed, pilot_key_t key);
	_pilot_signal(pilot_widget, focusChanged, pilot_bool_t in);
	_pilot_signal(pilot_widget, xChanged, pilot_coord_t x);
	_pilot_signal(pilot_widget, yChanged, pilot_coord_t y);
	pilot_bitsfield_t is_display:1;
	pilot_bitsfield_t hasfocus:1;
	pilot_bitsfield_t hasmouse:1;
	pilot_bitsfield_t hasclick:1;
	pilot_rect_t region;
	pilot_key_t key;
	pilot_pixel_format_t format;
	struct {
		void (*destroy)(void *widget);
		int (*show)(void *widget);
		int (*redraw)(void *widget);
		int (*resize)(void *widget, pilot_length_t width, pilot_length_t height);
	} action;
};

#define PILOT_INPUT_KEYBOARD 0
#define PILOT_INPUT_POINTER 1
#define PILOT_INPUT_TOUCH 2
struct pilot_input {
	struct pilot_display *display;
	pilot_key_t id;
	_pilot_signal(pilot_display, keyChanged, pilot_key_t key, pilot_bool_t state);
	_pilot_signal(pilot_display, mouseEntered, struct pilot_window *window, pilot_bool_t in);
	_pilot_signal(pilot_display, mouse_scrolled, pilot_coord_t x, pilot_coord_t y);
	_pilot_signal(pilot_display, mouse_clicked, pilot_key_t key, pilot_bool_t state);
	_pilot_signal(pilot_display, mouse_moved, pilot_coord_t x, pilot_coord_t y);
	void *platform;
};

struct pilot_display {
	struct pilot_widget common;
	_pilot_list(pilot_window, windows);
	_pilot_list(pilot_input, inputs);
	struct pilot_cursor *cursor;
	_pilot_signal(pilot_display, focusChanged, struct pilot_widget *window, pilot_bool_t in);
	_pilot_signal(pilot_display, inputChanged, struct pilot_input *input);
#ifdef HAVE_SUSPEND_RESUME
	_pilot_signal(pilot_display, suspended);
	_pilot_signal(pilot_display, resumed);
#endif
	pilot_length_t width, height;
	pilot_bitsfield_t formats;
	void *platform;
	void *egl;
};

struct pilot_surface {
	uint32_t size;
	uint32_t stride;
	pilot_length_t width;
	pilot_length_t height;
	pilot_pixel_format_t format;
	pilot_bool_t busy:1;
	pilot_bool_t ready:1;
	pilot_bool_t regionning:1;
	_pilot_mutex(lock);
	_pilot_cond(cond);
	void *data;
	struct {
		void (*destroy)(void *surface);
		void (*paint_window)(void *surface, struct pilot_window *window);
	} action;
};

struct pilot_fbuffer {
	struct pilot_surface common;
	struct pilot_display *display;
	int id;
	void *platform;
};

struct pilot_buffer {
	struct pilot_surface common;
	struct pilot_widget *parent;
	pilot_rect_t region;
	void *platform;
	
};

struct pilot_cursor {
	struct pilot_display *display;
	void *platform;
};

struct pilot_layout {
	struct pilot_widget common;
	int32_t maxwidgets;
	int32_t nbwidgets;
	struct pilot_widget *widgets[MAXWIDGETS];
	void *platform;
};

struct pilot_theme {
	struct pilot_window *window;
	struct pilot_widget *caption;
	pilot_length_t border;
	pilot_color_t bgcolor;
	pilot_bitsfield_t changed:1;
	pilot_bitsfield_t is_attached:1;
	void *platform;
};

struct pilot_window {
	struct pilot_widget common;
	struct pilot_surface *surfaces[MAXBUFFERS];
	struct pilot_theme *theme;
	struct pilot_widget *layout;
	pilot_length_t fullwidth, fullheight;
	pilot_bitsfield_t is_mainwindow:1;
	pilot_bitsfield_t fullscreen:1;
	pilot_bitsfield_t opaque:1;
	char *name;
	void *platform;
};

typedef int(*f_draw_handler)(void *draw_data, void *shm_buffer);
struct pilot_canvas
{
	struct pilot_widget common;
	struct pilot_buffer *buffers[MAXBUFFERS];
	struct pilot_buffer *next_buffer;
	_pilot_mutex(paintmutex);
	_pilot_signal(pilot_buffer, fliped);
	char	offscreenid:4;
	char	onscreenid:4;
	f_draw_handler draw_handler;
	void *draw_data;
	void *platform;
};

#ifdef HAVE_EGL
struct pilot_eglcanvas
{
	struct pilot_widget common;
	EGLSurface egl_surface;
	void *platform;
};
#endif

#define PILOT_RESOURCE_FORM 1
#define PILOT_RESOURCE_BUTTON 2
#define PILOT_RESOURCE_INPUT 3
#define PILOT_RESOURCE_SELECT 4
struct pilot_resource
{
	uint8_t type;
	uint8_t id;
	uint16_t unused;
	pilot_rect_t region;
	pilot_color_t *image;
};

struct pilot_form
{
	struct pilot_widget common;
	struct pilot_surface *surface;
	pilot_bool_t redraw;
	pilot_color_t *image;
	pilot_color_t *mask;
};

/**
 * pilot_widget API
 * **/
struct pilot_widget *
pilot_widget_create(struct pilot_widget *parent);
void
pilot_widget_destroy(struct pilot_widget *widget);
int
pilot_widget_init(struct pilot_widget *widget, struct pilot_widget *parent);
int
pilot_widget_show(struct pilot_widget *widget);
int
pilot_widget_redraw(struct pilot_widget *widget);
int
pilot_widget_resize(struct pilot_widget *widget,
						pilot_length_t width, pilot_length_t height);
int
pilot_widget_size(struct pilot_widget *widget,
						pilot_length_t *width, pilot_length_t *height);
int
pilot_widget_change_focus(struct pilot_widget *widget,
						struct pilot_widget *newwidgetchanged, char in);
void
pilot_widget_focus(struct pilot_widget *widget);
int
pilot_widget_grabkeys(struct pilot_widget *widget, pilot_bool_t yes);
#ifndef HAVE_INLINE
pilot_bool_t
pilot_widget_hasfocus(struct pilot_widget *widget);
pilot_bool_t
pilot_widget_hasmouse(struct pilot_widget *widget);
struct pilot_display *
pilot_widget_display(struct pilot_widget *widget);
#else
inline pilot_bool_t
pilot_widget_hasfocus(struct pilot_widget *widget){return widget->hasfocus;}
inline pilot_bool_t
pilot_widget_hasmouse(struct pilot_widget *widget){return widget->hasmouse;}
inline struct pilot_display *
pilot_widget_display(struct pilot_widget *widget){return widget->display;}
#endif
/**
 * pilot_display API
 * **/
struct pilot_display *
pilot_display_create(struct pilot_application *application);
void
pilot_display_destroy(struct pilot_display *display);
int
pilot_display_add_window(struct pilot_display *display, struct pilot_window *window);
int
pilot_display_mainloop(struct pilot_display *display);
int
pilot_display_exit(struct pilot_display *display, int ret);
#ifdef HAVE_SUSPEND_RESUME
int
pilot_display_suspend(struct pilot_display *display);
int
pilot_display_resume(struct pilot_display *display);
#endif
typedef pilot_bool_t (*f_search_handler)(struct pilot_window *);
typedef pilot_bool_t (*f_search_handler)(struct pilot_window *);
struct pilot_window *
pilot_display_search_window(struct pilot_display *display, f_search_handler search);
struct pilot_surface *
pilot_display_surface(struct pilot_display *display, pilot_rect_t region);
void
pilot_display_free_surface(struct pilot_display *display, struct pilot_surface *surface);
/**
 * pilot_input API
 * **/
struct pilot_input *
pilot_input_create(struct pilot_display *display, pilot_key_t id);
void
pilot_input_destroy(struct pilot_input *input);
/**
 * pilot_window API
 * **/
struct pilot_window *
pilot_window_create(struct pilot_widget *parent, char *name, pilot_length_t width, pilot_length_t height, struct pilot_theme *theme);
void
pilot_window_destroy(struct pilot_window *window);
int
pilot_window_detach(struct pilot_window *window, struct pilot_display *display);
int
pilot_window_set_layout(struct pilot_window *window, struct pilot_widget *layout);
struct pilot_widget *
pilot_window_layout(struct pilot_window *window);
struct pilot_surface *
pilot_window_surface(struct pilot_window *window, pilot_rect_t region);
int
pilot_window_show(struct pilot_window *window);
void
pilot_window_focus(struct pilot_window *window);
int
pilot_window_resize(struct pilot_window *widget, pilot_length_t width, pilot_length_t height);
/**
 * pilot_layout API
 * **/
struct pilot_layout *
pilot_layout_create(struct pilot_widget *parent);
void
pilot_layout_destroy(struct pilot_layout *layout);
int
pilot_layout_add_widget(struct pilot_layout *layout, struct pilot_widget *child);
int
pilot_canvas_lock(struct pilot_canvas *canvas, void **image);
void
pilot_canvas_unlock(struct pilot_canvas *canvas);
int
pilot_canvas_flip(struct pilot_canvas *canvas);
/**
 * pilot_canvas API
 * **/
struct pilot_canvas *
pilot_canvas_create(struct pilot_widget *parent, int format);
void
pilot_canvas_destroy(struct pilot_canvas *canvas);
int
pilot_canvas_set_draw_handler(struct pilot_canvas *canvas, f_draw_handler handler, void *data);
void *
pilot_canvas_draw_data(struct pilot_canvas *canvas);
int
pilot_canvas_lock(struct pilot_canvas *canvas, void **image);
void
pilot_canvas_unlock(struct pilot_canvas *canvas);
/**
 * pilot_surface API Abstract
 * **/
/*
 struct pilot_surface *
pilot_surface_create(pilot_rect_t rect,
				pilot_pixel_format_t format);
*/
void
pilot_surface_destroy(struct pilot_surface *surface);
int
pilot_surface_init(struct pilot_surface *surface,
				pilot_rect_t region,
				pilot_pixel_format_t format);
void
pilot_surface_paint_window(struct pilot_surface *surface, struct pilot_window *window);
int
pilot_surface_lock(struct pilot_surface *surface, void **image);
void 
pilot_surface_unlock(struct pilot_surface *surface);
void
pilot_surface_release(struct pilot_surface *surface);
#ifndef HAVE_INLINE
uint32_t
pilot_surface_size(struct pilot_surface *surface);
pilot_bool_t
pilot_surface_busy(struct pilot_surface *surface);
pilot_bool_t
pilot_surface_ready(struct pilot_surface *surface);
#else
inline uint32_t
pilot_surface_size(struct pilot_surface *surface){return surface->size;}
inline pilot_bool_t
pilot_surface_busy(struct pilot_surface *surface){return surface->busy;}
pilot_bool_t
pilot_surface_ready(struct pilot_surface *surface){return surface->ready;}
#endif
/**
 * pilot_fbuffer API
 * **/
struct pilot_fbuffer *
pilot_fbuffer_create(struct pilot_display *display,
				pilot_rect_t region,
				pilot_pixel_format_t pixel);
/*
void
pilot_fbuffer_destroy(struct pilot_fbuffer *fbuffer);
*/
/**
 * pilot_buffer API
 * **/
struct pilot_buffer *
pilot_buffer_create(struct pilot_widget *widget,
				pilot_rect_t region,
				pilot_pixel_format_t pixel);
/*
void
pilot_buffer_destroy(struct pilot_buffer *buffer);
*/
	/**
	 * pilot_buffer drawing API
	 */
	int
	pilot_buffer_set_region(struct pilot_buffer *buffer, pilot_rect_t *rect);
	int
	pilot_buffer_fill(struct pilot_buffer *buffer, pilot_color_t color);
/**
 * pilot_theme API
 * **/
struct pilot_theme *
pilot_theme_create(struct pilot_display *display);
struct pilot_theme *
pilot_theme_attach(struct pilot_theme *theme, struct pilot_window *window);
void
pilot_theme_destroy(struct pilot_theme *theme);
struct pilot_widget *
pilot_theme_get_caption(struct pilot_theme *theme);
pilot_length_t
pilot_theme_get_border(struct pilot_theme *theme);
int
pilot_theme_redraw_window(struct pilot_theme *theme);
int
pilot_theme_resize_window(struct pilot_theme *theme, pilot_length_t *width, pilot_length_t *height);

struct pilot_form *
pilot_form_create(struct pilot_widget *parent, int id);
void
pilot_form_destroy(struct pilot_form *form);

#endif
