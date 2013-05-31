#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <pilot_wtk.h>

static int running = 0;
static struct pilot_window *
_pilot_display_search_window(struct pilot_display *display, struct wl_surface *surface);
static int
_platform_display_create(struct pilot_display *display);
static void
_platform_display_destroy(struct pilot_display *display);

/**
 * display object
 **/
struct pilot_display *
pilot_display_create(void)
{
	struct pilot_display *display;

	display = malloc(sizeof(*display));
	if (!display)
		return NULL;
	memset(display, 0, sizeof(*display));

	pilot_widget_init(&display->common, NULL);
	display->common.display = display;
	display->common.is_display = 1;
	display->formats = 0;

	if (_platform_display_create(display) < 0) {
		free(display);
		return NULL;
	}

	return display;
}

void
pilot_display_destroy(struct pilot_display *display)
{
	_platform_display_destroy(display);
	free(display);
}
int
pilot_display_exit(struct pilot_display *display, int ret)
{
	running = 0;
	return ret;
}

int
pilot_display_add_window(struct pilot_display *display, struct pilot_window *window)
{
	int i;

	/**
	 * seach the first window free entry on display
	 **/
	typeof (display->windows) *windows_it = &display->windows;
	while (windows_it->next) windows_it = windows_it->next;
	windows_it->item = window;
	windows_it->next = malloc(sizeof(typeof (display->windows)));
	windows_it = windows_it->next;
	memset(windows_it, 0, sizeof(typeof (display->windows)));

	return 0;
}

int
pilot_display_mainloop(struct pilot_display *display)
{
	int ret = 0;

	running = 1;
	while (running && ret != -1)
		ret = wl_display_dispatch(display->platform.display);

	return ret;
}

struct pilot_window *
pilot_display_search_window(struct pilot_display *display, f_search_handler search)
{
	typeof (display->windows) *windows_it = &display->windows;
	while (windows_it->item) {
		if (search(windows_it->item)) break;
		windows_it = windows_it->next;
	}
	return windows_it->item;
}

static struct pilot_window *
_pilot_display_search_window(struct pilot_display *display, struct wl_surface *surface)
{
	typeof (display->windows) *windows_it = &display->windows;
	while (windows_it->item) {
		if (windows_it->item->platform.surface == surface) break;
		windows_it = windows_it->next;
	}
	return windows_it->item;
}

#include "platform_display.c"
