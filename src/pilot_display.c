#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <pilot_wtk.h>
#include <pilot_atk.h>

static int running = 0;
static int
_platform_display_create(struct pilot_display *display,
							struct pilot_connector *connector);
static void
_platform_display_destroy(struct pilot_display *display);
static int
_platform_display_prepare_wait(struct pilot_display *display);
static int
_platform_display_dispatch_events(struct pilot_display *display);
int
_platform_display_region(struct pilot_display *display,
						pilot_rect_t *region);
static pilot_pixel_format_t
_platform_display_format(struct pilot_display *display);

/**
 * display object
 **/
struct pilot_display *
pilot_display_create(struct pilot_application *application)
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

	struct pilot_connector *connector = 
		pilot_connector_create(display);
	if (_platform_display_create(display, connector) < 0) {
		LOG_ERROR("platform not available");
		free(display);
		return NULL;
	}
	connector->action.prepare_wait = _platform_display_prepare_wait;
	connector->action.dispatch_events = _platform_display_dispatch_events;
	pilot_application_addconnector(application, connector);

	return display;
}

void
pilot_display_destroy(struct pilot_display *display)
{
	_platform_display_destroy(display);
	free(display);
}

int
pilot_display_add_window(struct pilot_display *display, struct pilot_window *window)
{
	if (!window->is_mainwindow) {
		pilot_window_detach(window, display);
	}
	pilot_connect(display, focusChanged, (struct pilot_widget *)window, pilot_widget_change_focus);
	pilot_list_append(display->windows, window);
	return 0;
}

int
pilot_display_add_input(struct pilot_display *display, struct pilot_input *input)
{
	LOG_DEBUG("");
	pilot_list_append(display->inputs, input);
	pilot_emit(display, inputChanged, input);
	return 0;
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

struct pilot_surface *
pilot_display_surface(struct pilot_display *display, pilot_rect_t region)
{
	struct pilot_fbuffer *fbuffer;
	pilot_pixel_format_t format;

	_platform_display_region(display, &region);
	format = _platform_display_format(display);
	fbuffer = pilot_fbuffer_create(display, region, format);

	return (struct pilot_surface *)fbuffer;
}

#include "platform_display.c"
