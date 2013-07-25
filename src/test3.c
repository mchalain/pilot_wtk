
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pilot_wtk.h>
#include <pilot_atk.h>
#include <pilot_utk.h>
#include <linux/input.h>
#include <dlfcn.h>
#include "test3.res.h"

struct pilot_application *g_application;

int
mainwindow_init(struct pilot_window *mainwindow)
{
	struct pilot_form *form;
	
	form = pilot_form_create((struct pilot_widget *)mainwindow, MAINFORM);
	if (!form)
		return -1;

	pilot_window_set_layout(mainwindow, (struct pilot_widget*)form);
	return 0;
}

void
mainwindow_fini(struct pilot_window *mainwindow)
{
}

int main(int argc, char **argv)
{
	int ret = 0;
	struct pilot_display *display;
	struct pilot_window *mainwindow;
	struct pilot_theme *theme = NULL;

	/**
	 * Setup
	 **/
	g_application = pilot_application_create(argc, argv);
	display = pilot_display_create(g_application);
	//theme = pilot_theme_create(display);
	
	mainwindow = pilot_window_create((struct pilot_widget *)display, "mainwindow", PILOT_DISPLAY_WIDTH, PILOT_DISPLAY_HEIGHT, theme);
	if (!mainwindow)
		return -1;
	if (mainwindow_init(mainwindow) < 0)
		return -1;
	pilot_display_add_window(display, mainwindow);

	pilot_window_show(mainwindow);

	/**
	 * MainLoop
	 **/
	ret = pilot_application_run(g_application);

	/**
	 * Cleanup
	 **/
	mainwindow_fini(mainwindow);
	pilot_window_destroy(mainwindow);
	pilot_display_destroy(display);
	pilot_application_destroy(g_application);
	return ret;
}
