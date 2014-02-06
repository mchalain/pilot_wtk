#ifndef __PILOT_WTK_H__
#define __PILOT_WTK_H__

#include <stdint.h>
#include <pilot_types.h>
#include <pilot_log.h>
#include <pilot_signal.h>
#include <pilot_list.h>

#define MAXBUFFERS 2
#define MAXWIDGETS 4

struct pilot_application;
struct pilot_connector;
struct pilot_window;
struct pilot_surface;

typedef enum
{
	PILOT_DISPLAY_ARGB8888,
	PILOT_DISPLAY_XRGB8888
} pilot_pixel_format_t;

struct pilot_display {
	struct pilot_connector *connector;
	struct pilot_connector *synchconnector;
	int synchfd[2];
	_pilot_signal(pilot_display, synch);
	_pilot_list(pilot_window, windows);
	pilot_length_t width, height;
	int formats;
	pilot_bool_t force_redraw :1;
	void *platform;
};

struct pilot_window {
	struct pilot_display *display;
	struct pilot_surface *surface;
	struct pilot_rect drawingrect;
	pilot_length_t fullwidth, fullheight;
	pilot_bitsfield_t is_mainwindow:1;
	pilot_bitsfield_t fullscreen:1;
	pilot_bitsfield_t opaque:1;
	char *name;
	struct {
		void (*destroy)(struct pilot_window *window);
		int (*show)(struct pilot_window *window);
		int (*redraw)(struct pilot_window *window);
		int (*resize)(struct pilot_window *window, pilot_length_t width, pilot_length_t height);
	} action;
	void *platform;
};

/**
 * pilot_display API
 * **/
struct pilot_display *
pilot_display_create(struct pilot_application *application);
void
pilot_display_destroy(struct pilot_display *display);
int
pilot_display_exit(struct pilot_display *display, int ret);
pilot_pixel_format_t
pilot_display_format(struct pilot_display *display);
/**
 * pilot_window API
 * **/
struct pilot_window *
pilot_window_create(struct pilot_display *display, char *name, struct pilot_rect rect);
void
pilot_window_destroy(struct pilot_window *window);
int
pilot_window_show(struct pilot_window *window);

#endif
