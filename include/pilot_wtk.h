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
struct pilot_widget;
struct pilot_blit;

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

typedef enum
{
	EWidgetWindow,
	EWidgetCommon,
} pilot_widget_type_t;

struct pilot_window {
	struct pilot_display *display;
	pilot_widget_type_t type;
	struct pilot_rect drawingrect;
	struct {
		void (*destroy)(struct pilot_window *thiz);
		int (*show)(struct pilot_window *thiz);
		int (*redraw)(struct pilot_window *thiz);
		int (*resize)(struct pilot_window *thiz, pilot_length_t width, pilot_length_t height);
	} action;
	struct pilot_widget *layout;
	struct pilot_surface *surface;
	pilot_bitsfield_t fullscreen:1;
	pilot_bitsfield_t opaque:1;
	pilot_bool_t force_redraw :1;
	char *name;
	void *platform;
};

struct pilot_widget {
	struct pilot_widget *parent;
	pilot_widget_type_t type;
	struct pilot_rect drawingrect;
	struct {
		void (*destroy)(struct pilot_widget *thiz);
		int (*show)(struct pilot_widget *thiz);
		int (*redraw)(struct pilot_widget *thiz, struct pilot_blit *blit);
		int (*resize)(struct pilot_widget *thiz, pilot_length_t width, pilot_length_t height);
	} action;
	_pilot_list(pilot_widget, childs);
	void *widget;
};

/**
 * pilot_display API
 * **/
struct pilot_display *
pilot_display_create(struct pilot_application *application);
void
pilot_display_destroy(struct pilot_display *display);
int
pilot_display_exit(struct pilot_display *thiz, int ret);
pilot_pixel_format_t
pilot_display_format(struct pilot_display *thiz);
/**
 * pilot_window API
 * **/
struct pilot_window *
pilot_window_create(struct pilot_display *display, char *name, struct pilot_rect rect);
void
pilot_window_destroy(struct pilot_window *thiz);
int
pilot_window_show(struct pilot_window *thiz);

/**
 * pilot_widget API
 * **/
struct pilot_widget *
pilot_widget_create(struct pilot_widget *parent, struct pilot_rect rect);
void
pilot_widget_destroy(struct pilot_widget *thiz);

typedef int(*f_draw_handler)(void *draw_data, struct pilot_blit *blit);
struct pilot_widget *
pilot_canvas_create(struct pilot_widget *parent);
int
pilot_canvas_set_draw_handler(struct pilot_widget *thiz, f_draw_handler handler, void *data);

#endif
