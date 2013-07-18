#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pilot_atk.h>
#include <pilot_wtk.h>
#include <linux/input.h>

struct pilot_application *g_application;

int
mainwindow_init(struct pilot_window *mainwindow)
{
	return 0;
}

void
mainwindow_fini(struct pilot_window *mainwindow)
{
}

int keypressed(struct pilot_widget *widget, pilot_key_t key)
{
	if (key == KEY_ESC)
		pilot_application_exit(g_application, 0);
	else {
		pilot_widget_redraw(widget);
	}
	return 0;
}
int click(struct pilot_widget *widget, pilot_key_t key)
{
	printf("click %d\n", key);
	
	return 0;
}
int main_window_focus(struct pilot_widget *window, pilot_bool_t in)
{
	pilot_widget_grabkeys((struct pilot_widget *)window, in);
	if (in)
	{
		printf("%p I have the focus\n", window);
		pilot_connect(window, keyPressed, window, keypressed);
	}
	else
	{
		printf("%p I lost the focus\n", window);
		pilot_disconnect(window, keyPressed, window, keypressed);
	}
}

int main(int argc, char **argv)
{
	int ret = 0;
	struct pilot_display *display;
	struct pilot_window *mainwindow;
	struct pilot_theme *theme;

	/**
	 * Setup
	 **/
	g_application = pilot_application_create(argc, argv);
	display = pilot_display_create(g_application);

	theme = pilot_theme_create(display);

	mainwindow = pilot_window_create((struct pilot_widget *)display, "coucou", 500, 500, theme);
	if (!mainwindow)
		return -1;
	mainwindow_init(mainwindow);

	pilot_display_add_window(display, mainwindow);

	struct pilot_widget *mainwidget = (struct pilot_widget *)mainwindow;
	pilot_connect(mainwidget, focusChanged, mainwidget, main_window_focus);

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
	pilot_theme_destroy(theme);
	pilot_display_destroy(display);
	pilot_application_destroy(g_application);
	return ret;
}
