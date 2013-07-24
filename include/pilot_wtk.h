#ifndef __PILOT_WTK_H__
#define __PILOT_WTK_H__

#include <stdint.h>
#include "pilot_types.h"

#define MAXWINDOWS 3
#define MAXBUFFERS 2
#define MAXWIDGETS 4
#define MAXSIGNALS 4
#define MAXSLOTS 4

#include "pilot_log.h"
struct pilot_application;
struct pilot_window;
struct pilot_eglcanvas;
struct pilot_canvas;
struct pilot_cursor;
struct pilot_theme;

#ifndef _pilot_signal
#define _pilot_signal(obj, signal,...) \
	struct { \
		struct obj##_##signal##_slot{ \
			int(*action)(struct pilot_widget *, ##__VA_ARGS__); \
			struct pilot_widget *dest; \
			struct obj##_##signal##_slot *next; \
		} slots; \
	} signal

#define pilot_connect(src, signal, dst, slot) \
	do { \
		typeof(src->signal.slots) *slot_it = &src->signal.slots; \
		while (slot_it->next) { \
			if ((void *)slot_it->next->dest == (void *)dst && (void *)slot_it->next->action == (void *)slot) \
				break; \
			slot_it = slot_it->next; \
		} \
		if (!slot_it->next) { \
			slot_it->next = malloc(sizeof(src->signal.slots)); \
			memset(slot_it->next, 0, sizeof(src->signal.slots)); \
			slot_it->next->dest = (struct pilot_widget *)dst; \
			slot_it->next->action = slot; \
		} \
	} while(0)

#define pilot_disconnect(src, signal, dst, slot) \
	do { \
		typeof(src->signal.slots) *slot_it = &src->signal.slots; \
		while (slot_it->next) { \
			if ((void *)slot_it->next->dest == (void *)dst && (void *)slot_it->next->action == (void *)slot) { \
				typeof(src->signal.slots) *tmp = slot_it->next; \
				slot_it->next = tmp->next; \
				free(tmp); \
			} \
			else \
				slot_it = slot_it->next; \
		} \
	} while(0)

#define pilot_emit(src, signal, ...) \
	do { \
		typeof(src->signal.slots) *slot_it = &src->signal.slots; \
		while (slot_it->next) { \
			slot_it = slot_it->next; \
			slot_it->action(slot_it->dest, ##__VA_ARGS__); \
		} \
	} while(0)

#define pilot_emit_to(src, signal, dst, ...) \
	do { \
		typeof(src->signal.slots) *slot_it = &src->signal.slots; \
		while (slot_it->next) { \
			slot_it = slot_it->next; \
			if ((void *)dst == (void *)slot_it->dest) \
				slot_it->action(slot_it->dest, ##__VA_ARGS__); \
		} \
	} while(0)
#endif

#define PILOT_DISPLAY_ARGB8888 0
#define PILOT_DISPLAY_XRGB8888 1

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
	pilot_length_t width, height;
	pilot_coord_t x, y;
	pilot_key_t key;
	int format;
	struct {
		void (*destroy)(void *widget);
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

struct pilot_display {
	struct pilot_widget common;
	_pilot_list(pilot_window, windows);
	_pilot_list(pilot_input, inputs);
	struct pilot_cursor *cursor;
	_pilot_signal(pilot_display, focusChanged, struct pilot_window *window, pilot_bool_t in);
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
	struct pilot_buffer *buffer;
	struct pilot_widget *caption;
	pilot_length_t border;
	pilot_color_t bgcolor;
	void *platform;
};

struct pilot_window {
	struct pilot_widget common;
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

struct pilot_buffer {
	struct pilot_widget *parent;
	uint32_t size;
	void *shm_data;
	pilot_bool_t busy:1;
	pilot_bool_t ready:1;
	_pilot_mutex(lock);
	_pilot_cond(cond);
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
pilot_widget_redraw(struct pilot_widget *widget);
int
pilot_widget_size(struct pilot_widget *widget, pilot_length_t *width, pilot_length_t *height);
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
pilot_window_set_layout(struct pilot_window *window, struct pilot_widget *layout);
struct pilot_widget *
pilot_window_layout(struct pilot_window *window);
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
 * pilot_buffer API
 * **/
struct pilot_buffer *
pilot_buffer_create(struct pilot_widget *widget);
void
pilot_buffer_destroy(struct pilot_buffer *buffer);
void
pilot_buffer_paint_window(struct pilot_buffer *buffer, struct pilot_window *window);
int
pilot_buffer_lock(struct pilot_buffer *buffer, void **image);
void 
pilot_buffer_unlock(struct pilot_buffer *buffer);
void
pilot_buffer_release(struct pilot_buffer *buffer);
#ifndef HAVE_INLINE
uint32_t
pilot_buffer_size(struct pilot_buffer *buffer);
pilot_bool_t
pilot_buffer_busy(struct pilot_buffer *buffer);
pilot_bool_t
pilot_buffer_ready(struct pilot_buffer *buffer);
#else
inline uint32_t
pilot_buffer_size(struct pilot_buffer *buffer){return buffer->size;}
inline pilot_bool_t
pilot_buffer_busy(struct pilot_buffer *buffer){return buffer->busy;}
pilot_bool_t
pilot_buffer_ready(struct pilot_buffer *buffer){return buffer->ready;}
#endif
/**
 * pilot_theme API
 * **/
struct pilot_theme *
pilot_theme_create(struct pilot_display *display);
struct pilot_theme *
pilot_theme_duplicate(struct pilot_theme *theme);
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


#endif
