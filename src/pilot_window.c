#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pilot_wtk.h>

static void
_pilot_window_destroy(void *widget);
static int
_pilot_window_redraw(void *widget);
static int
_pilot_window_resize(void *widget, uint32_t width, uint32_t height);
static int
_platform_window_flush(struct pilot_window *window);
static int 
_platform_window_create(struct pilot_window *window, struct pilot_display *display);
static void
_platform_window_destroy(struct pilot_window *window);
static int
_pilot_window_grabkeys(struct pilot_widget *widget, struct pilot_window *window, char in);

struct pilot_window *
pilot_window_create(struct pilot_widget *widget, char *name, uint32_t width, uint32_t height)
{
	struct pilot_window *window;
	struct pilot_display *display;

	window = calloc(1, sizeof *window);
	if (!window)
		return NULL;
	memset(window, 0, sizeof(*window));

	pilot_widget_init(&window->common, widget);
	display = window->common.display;
	
	window->fullwidth = width;
	window->fullheight = height;
	//window->theme = pilot_theme_create(display);
	if (window->theme)
		pilot_theme_resize_window(window->theme, &width, &height);
	window->common.width = width;
	window->common.height = height;

	window->common.action.redraw = _pilot_window_redraw;
	window->common.action.resize = _pilot_window_resize;
	window->common.action.destroy = _pilot_window_destroy;

	if (name) {
		window->name = malloc(strlen(name) + 1);
		strcpy(window->name, name);
	}

	_platform_window_create(window, display);

	pilot_connect(display, focusChanged, (struct pilot_widget *)window, _pilot_window_grabkeys);
	return window;
}

static void
_pilot_window_destroy(void *widget)
{
	struct pilot_window *window = widget;
	if (window->theme)
		pilot_theme_destroy(window->theme);
	pilot_window_destroy(window);
}

void
pilot_window_destroy(struct pilot_window *window)
{
	if (window->layout)
		pilot_widget_destroy(window->layout);
	if (window->name)
		free(window->name);

	_platform_window_destroy(window);
	free(window);
}

int
pilot_window_show(struct pilot_window *window)
{
	wl_shell_surface_set_toplevel(window->platform.shell_surface);

	if (window->platform.callback)
		return 0;
	_pilot_window_redraw(window);

	return 0;
}


static int
_pilot_window_grabkeys(struct pilot_widget *widget, struct pilot_window *window, char in)
{
	if ((void *)widget == (void *)window)
	{
		pilot_emit(widget, focusChanged, in);
		return pilot_widget_grabkeys((struct pilot_widget *)window, in);
	}
	return 0;
}

void
pilot_window_focus(struct pilot_window *window)
{
	struct pilot_window *old = pilot_display_search_window(window->common.display, (f_search_handler)pilot_widget_hasfocus);
	pilot_emit(window->common.display, focusChanged, old, 0);
	pilot_emit(window->common.display, focusChanged, window, 1);
}

int
pilot_window_set_layout(struct pilot_window *window, struct pilot_widget *layout)
{
	if (!window->layout)
		window->layout = layout;
	return -(!(window->layout == layout));
}

struct pilot_widget *
pilot_window_layout(struct pilot_window *window)
{
	if (!window->layout)
		window->layout = (struct pilot_widget *)pilot_layout_create((struct pilot_widget *)window);
	return window->layout;
}

int
pilot_window_resize(struct pilot_window *window, uint32_t width, uint32_t height)
{
	window->common.width = width;
	window->common.height = height;
	return 0;
}

int
pilot_window_fullscreen(struct pilot_window *window)
{
	return 0;
}

static int
_pilot_window_redraw(void *widget)
{
	struct pilot_window *window = widget;
	
	if (window->layout) {
		pilot_widget_redraw(window->layout);
	}
	
	return _platform_window_flush(window);
}
static int
_pilot_window_resize(void *widget, uint32_t width, uint32_t height)
{
	struct pilot_window *window = widget;

	window->fullwidth = width;
	window->fullheight = height;
	if (window->theme)
		pilot_theme_resize_window(window->theme, &width, &height);
	if (window->layout && window->layout->action.resize)
		window->layout->action.resize(window->layout, width, height);

	window->common.width = width;
	window->common.height = height;

	return 0;
}

#include "platform_window.c"
