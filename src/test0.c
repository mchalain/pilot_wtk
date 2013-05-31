#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pilot_wtk.h>
#include <linux/input.h>

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
		pilot_display_exit(pilot_widget_display(widget), 0);
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

	/**
	 * Setup
	 **/
	display = pilot_display_create();

	mainwindow = pilot_window_create((struct pilot_widget *)display, "coucou", 500, 500);
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
	ret = pilot_display_mainloop(display);

	/**
	 * Cleanup
	 **/
	mainwindow_fini(mainwindow);
	pilot_window_destroy(mainwindow);
	pilot_display_destroy(display);
	return ret;
}
