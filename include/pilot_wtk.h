#ifndef __PILOT_WTK_H__
#define __PILOT_WTK_H__

#include <stdint.h>
#include <wayland-client.h>

#define MAXWINDOWS 3
#define MAXBUFFERS 2
#define MAXWIDGETS 4
#define MAXSIGNALS 4
#define MAXSLOTS 4

typedef int32_t pilot_coord_t;
typedef uint32_t pilot_length_t;
typedef uint32_t pilot_bitsfield_t;
typedef uint32_t pilot_color_t;
typedef int16_t pilot_key_t;
typedef char pilot_bool_t;
typedef char pilot_mutex_t;

struct pilot_window;
struct pilot_eglcanvas;
struct pilot_canvas;
struct pilot_cursor;
struct pilot_theme;

struct pilot_theme {
	struct pilot_widget *caption;
	int border;
};

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

struct pilot_widget {
	struct pilot_display *display;
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

struct pilot_display {
	struct pilot_widget common;
	struct _pilot_display_windows{
		struct pilot_window *item;
		struct _pilot_display_windows *next;
	} windows;
	struct pilot_cursor *cursor;
	_pilot_signal(pilot_display, keyChanged, pilot_key_t key, pilot_bool_t state);
	_pilot_signal(pilot_display, focusChanged, struct pilot_window *window, pilot_bool_t in);
	_pilot_signal(pilot_display, mouseEntered, struct pilot_window *window, pilot_bool_t in);
	_pilot_signal(pilot_display, mouse_scrolled, pilot_coord_t x, pilot_coord_t y);
	_pilot_signal(pilot_display, mouse_clicked, pilot_key_t key, pilot_bool_t state);
	_pilot_signal(pilot_display, mouse_moved, pilot_coord_t x, pilot_coord_t y);
	struct {
		struct wl_display *display;
		struct wl_registry *registry;
		struct wl_compositor *compositor;
		struct wl_shell *shell;
		struct wl_shm *shm;
		struct wl_seat *seat;
		struct wl_pointer *pointer;
		struct wl_keyboard *keyboard;
	} platform;
	pilot_bitsfield_t formats;
	void *egl;
};

struct pilot_cursor {
	struct pilot_display *display;
	struct {
		struct wl_cursor *cursor;
		struct wl_surface *surface;
		struct wl_cursor_theme *theme;
	} platform;
};

struct pilot_buffer {
	struct pilot_widget *parent;
	pilot_mutex_t busy;
	uint32_t size;
	void *shm_data;
	struct {
		struct wl_buffer *buffer;
	} platform;
};

struct pilot_layout {
	struct pilot_widget common;
	int32_t maxwidgets;
	int32_t nbwidgets;
	struct pilot_widget *widgets[MAXWIDGETS];
};

struct pilot_window {
	struct pilot_widget common;
	struct pilot_theme *theme;
	struct pilot_widget *layout;
	pilot_length_t fullwidth, fullheight;
	pilot_bitsfield_t fullscreen:1;
	pilot_bitsfield_t opaque:1;
	char *name;
	struct {
		struct wl_surface *surface;
		struct wl_shell_surface *shell_surface;
		struct wl_callback *callback;
	} platform;
};

typedef int(*f_draw_handler)(void *draw_data, void *shm_buffer);
struct pilot_canvas
{
	struct pilot_widget common;
	struct pilot_buffer *buffers[MAXBUFFERS];
	struct pilot_buffer *next_buffer;
	f_draw_handler draw_handler;
	void *draw_data;
};

#ifdef HAVE_EGL
struct pilot_eglcanvas
{
	struct pilot_widget common;
	struct {
		struct wl_egl_window *native;
	} platform;
	EGLSurface egl_surface;
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
pilot_display_create(void);
void
pilot_display_destroy(struct pilot_display *display);
int
pilot_display_add_window(struct pilot_display *display, struct pilot_window *window);
int
pilot_display_mainloop(struct pilot_display *display);
int
pilot_display_exit(struct pilot_display *display, int ret);
typedef pilot_bool_t (*f_search_handler)(struct pilot_window *);
struct pilot_window *
pilot_display_search_window(struct pilot_display *display, f_search_handler search);
/**
 * pilot_window API
 * **/
struct pilot_window *
pilot_window_create(struct pilot_widget *parent, char *name, pilot_length_t width, pilot_length_t height);
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
/**
 * pilot_buffer API
 * **/
struct pilot_buffer *
pilot_buffer_create(struct pilot_widget *widget);
void
pilot_buffer_destroy(struct pilot_buffer *buffer);
void
pilot_buffer_paint_window(struct pilot_buffer *buffer, struct pilot_window *window);
#ifndef HAVE_INLINE
uint32_t
pilot_buffer_size(struct pilot_buffer *buffer);
void
pilot_buffer_release(struct pilot_buffer *buffer);
pilot_mutex_t
pilot_buffer_busy(struct pilot_buffer *buffer);
#else
inline uint32_t
pilot_buffer_size(struct pilot_buffer *buffer){return buffer->size;}
inline void
pilot_buffer_release(struct pilot_buffer *buffer){buffer->busy = 0;}
inline pilot_mutex_t
pilot_buffer_busy(struct pilot_buffer *buffer){return buffer->busy;}
#endif
/**
 * pilot_theme API
 * **/
struct pilot_theme *
pilot_theme_create(struct pilot_window *window);
void
pilot_theme_destroy(struct pilot_theme *theme);
struct pilot_widget *
pilot_theme_get_caption(struct pilot_theme *theme);
pilot_length_t
pilot_theme_get_border(struct pilot_theme *theme);
int
pilot_theme_resize_window(struct pilot_theme *theme, pilot_length_t *width, pilot_length_t *height);


#endif
