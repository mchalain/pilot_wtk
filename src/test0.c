#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pilot_atk.h>
#include <pilot_wtk.h>
#include <linux/input.h>

struct pilot_application *g_application;

int main(int argc, char **argv)
{
	int ret = 0;
	struct pilot_display *display;
	struct pilot_window *mainwindow;
	pilot_rect_t rect = { .w = 500, .h = 550 };

	/**
	 * Setup
	 **/
	g_application = pilot_application_create(argc, argv);
	display = pilot_display_create(g_application);

	mainwindow = pilot_window_create(display, "coucou", rect);
	pilot_window_show(mainwindow);

	/**
	 * MainLoop
	 **/
	ret = pilot_application_run(g_application);

	/**
	 * Cleanup
	 **/
	pilot_window_destroy(mainwindow);
	pilot_display_destroy(display);
	pilot_application_destroy(g_application);
	return ret;
}
