#ifndef __PILOT_WTK_H__
#define __PILOT_WTK_H__

#include <stdint.h>
#include <wayland-client.h>

#define MAXWINDOWS 3
#define MAXBUFFERS 2
struct pilot_window;
struct pilot_eglcanvas;
struct pilot_canvas;

struct pilot_display {
	struct wl_display *display;
	struct wl_registry *registry;
	struct wl_compositor *compositor;
	struct wl_shell *shell;
	struct wl_shm *shm;
	uint32_t formats;
	struct pilot_window *windows[MAXWINDOWS];
};

struct pilot_buffer {
	struct wl_buffer *buffer;
	void *shm_data;
	int busy;
};

struct pilot_window {
	struct pilot_display *display;
	int width, height;
	struct wl_surface *surface;
	struct wl_shell_surface *shell_surface;
	struct pilot_buffer *buffers[MAXBUFFERS];
	struct pilot_buffer *next_buffer;
	struct wl_callback *callback;
	struct pilot_canvas *canvas;
	struct pilot_eglcanvas *eglcanvas;
};

typedef int(*f_draw_handler)(void *draw_data, void *shm_buffer);
struct pilot_canvas
{
	struct pilot_window *window;
	int width, height, format;
	f_draw_handler *draw_handler;
	void *draw_data;
};

struct pilot_eglcanvas
{
};
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
/**
 * pilot_window API
 * **/
struct pilot_window *
pilot_window_create(struct pilot_display *display, char *name, int width, int height);
void
pilot_window_destroy(struct pilot_window *window);
int
pilot_window_set_canvas(struct pilot_window *window, struct pilot_canvas *canvas);
int
pilot_window_redraw(struct pilot_window *window);
/**
 * pilot_canvas API
 * **/
struct pilot_canvas *
pilot_canvas_create(struct pilot_window *window, int format);
void
pilot_canvas_destroy(struct pilot_canvas *canvas);
int
pilot_canvas_set_draw_handler(struct pilot_canvas *canvas, f_draw_handler *handler, void *data);
/**
 * pilot_buffer API
 * **/
struct pilot_buffer *
pilot_buffer_create(struct pilot_display *display,
		  int width, int height, uint32_t format);
void
pilot_buffer_destroy(struct pilot_buffer *buffer);


#endif
