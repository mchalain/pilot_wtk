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
static const struct wl_callback_listener _st_frame_listener;
static const struct wl_shell_surface_listener _st_shell_surface_listener;
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

	window->platform.callback = NULL;
	window->platform.surface = wl_compositor_create_surface(display->platform.compositor);
	window->platform.shell_surface = wl_shell_get_shell_surface(display->platform.shell,
							   window->platform.surface);

	if (window->platform.shell_surface)
		wl_shell_surface_add_listener(window->platform.shell_surface,
					      &_st_shell_surface_listener, window);

	if (name)
		wl_shell_surface_set_title(window->platform.shell_surface, name);

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
	int i;
	if (window->platform.callback)
		wl_callback_destroy(window->platform.callback);

	if (window->layout)
		 pilot_widget_destroy(window->layout);

	wl_shell_surface_destroy(window->platform.shell_surface);
	wl_surface_destroy(window->platform.surface);
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
	wl_surface_damage(window->platform.surface,
			  0, 0, window->fullwidth, window->fullheight);

	window->platform.callback = wl_surface_frame(window->platform.surface);
	wl_callback_add_listener(window->platform.callback, &_st_frame_listener, window);
	wl_surface_commit(window->platform.surface);
	return 0;
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

static void
_pilot_window_surface_ping_handler(void *data, struct wl_shell_surface *shell_surface,
							uint32_t serial)
{
	wl_shell_surface_pong(shell_surface, serial);
}

static void
_pilot_window_surface_configure_handler(void *data, struct wl_shell_surface *shell_surface,
		 uint32_t edges, int32_t width, int32_t height)
{
	_pilot_window_resize(data, width, height);
}

static void
_pilot_window_surface_popup_done_handler(void *data, struct wl_shell_surface *shell_surface)
{
}

static const struct wl_shell_surface_listener _st_shell_surface_listener = {
	_pilot_window_surface_ping_handler,
	_pilot_window_surface_configure_handler,
	_pilot_window_surface_popup_done_handler
};

static void
_pilot_window_frame_handler(void *data, struct wl_callback *callback, uint32_t time)
{
	struct pilot_window *window = data;

	if (callback != window->platform.callback)
		return;
	window->platform.callback = NULL;

	if (callback)
		wl_callback_destroy(callback);

	_pilot_window_redraw(data);

	window->platform.callback = wl_surface_frame(window->platform.surface);
	wl_callback_add_listener(window->platform.callback, &_st_frame_listener, window);

}

static const struct wl_callback_listener _st_frame_listener = {
	_pilot_window_frame_handler
};
