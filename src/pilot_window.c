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
static struct pilot_surface *
_platform_window_surface(struct pilot_window *window, pilot_rect_t region);
static int
_pilot_window_focusChanged(struct pilot_widget *widget, struct pilot_window *window, char in);
static int
_pilot_window_installinput(struct pilot_widget *widget, struct pilot_input *input);

struct pilot_window *
pilot_window_create(struct pilot_widget *parent, char *name, uint32_t width, uint32_t height, struct pilot_theme *theme)
{
	struct pilot_window *window;
	struct pilot_display *display;

	window = calloc(1, sizeof *window);
	if (!window)
		return NULL;
	memset(window, 0, sizeof(*window));

	pilot_widget_init(&window->common, parent);

	window->fullwidth = width;
	window->fullheight = height;
	window->common.region.x = 0;
	window->common.region.y = 0;
	window->common.region.w = width;
	window->common.region.h = height;
	window->theme = theme;
	LOG_DEBUG("rect %d x %d", width, height);

	window->common.action.redraw = _pilot_window_redraw;
	window->common.action.resize = _pilot_window_resize;
	window->common.action.destroy = _pilot_window_destroy;

	if (name) {
		window->name = malloc(strlen(name) + 1);
		strcpy(window->name, name);
	}

	if (parent && parent->is_display) {
		pilot_window_detach(window, (struct pilot_display *)parent);
	}
	return window;
}

static void
_pilot_window_destroy(void *widget)
{
	struct pilot_window *window = widget;
	if (window->surfaces[0]) {
		pilot_surface_destroy(window->surfaces[0]);
	}
	if (window->surfaces[1]) {
		pilot_surface_destroy(window->surfaces[1]);
	}
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

	if (window->is_mainwindow)
		_platform_window_destroy(window);
	free(window);
}

int
pilot_window_detach(struct pilot_window *window, struct pilot_display *display)
{
	LOG_DEBUG("");
	window->common.display = display;
	window->common.window = window;
	window->is_mainwindow = 1;
	window->opaque = 1;
	window->surfaces[0] = pilot_display_surface(window->common.display,
							window->common.region);
	if (window->theme && !window->theme->is_attached) {
		window->theme = pilot_theme_attach(window->theme, window);
	}
	_platform_window_create(window, window->common.display);
	return 0;
}

int
pilot_window_show(struct pilot_window *window)
{
	int ret = 0;
	/*TODO*/
	if (window->theme) {
		pilot_theme_redraw_window(window->theme);
	}
	if (window->layout) {
		pilot_widget_show(window->layout);
	}
	if (window->is_mainwindow) {
		pilot_surface_paint_window(window->surfaces[0], window);
		return _platform_window_flush(window);
	}
	return ret;
}

int
pilot_window_set_layout(struct pilot_window *window, struct pilot_widget *layout)
{
	if (!window->layout) {
		window->layout = layout;
		pilot_widget_resize((struct pilot_widget *)window->layout,
						window->common.region.w,
						window->common.region.h);
	}
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
	_pilot_window_resize(window, width, height);
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
	int ret = 0;
	struct pilot_window *window = widget;

	if (window->theme) {
		ret = pilot_theme_redraw_window(window->theme);
	}
	if (window->layout) {
	LOG_DEBUG("layout %p",window->layout);
		ret += pilot_widget_redraw(window->layout);
	}
	LOG_DEBUG("ret %d",ret);
	if (ret && window->is_mainwindow) {
		return _platform_window_flush(window);
	}
	return ret;
}

static int
_pilot_window_resize(void *widget, uint32_t width, uint32_t height)
{
	int ret = 0;
	struct pilot_window *window = widget;

	window->fullwidth = width;
	window->fullheight = height;
	if (window->theme) {
		struct pilot_theme *theme = window->theme;
		window->theme = pilot_theme_attach(theme, window);
		pilot_theme_destroy(theme);
	} else {
		window->common.region.w = width;
		window->common.region.h = height;
	}
	if (window->layout && window->layout->action.resize)
		ret = window->layout->action.resize(window->layout, 
						window->common.region.w,
						window->common.region.h);
	LOG_DEBUG("ret %d",ret);
	_pilot_window_redraw(widget);
	return ret;
}

struct pilot_surface *
pilot_window_surface(struct pilot_window *window, pilot_rect_t region)
{
	struct pilot_surface *surface;
	surface = _platform_window_surface(window, region);
	return surface;
}

#include "platform_window.c"
