#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pilot_wtk.h>
#include <linux/input.h>

struct mycanvas_data
{
	struct pilot_canvas *canvas;
	struct pilot_window *window;
	int change;
} canvas_data;

static void
paint_pixels(void *image, int padding, int width, int height, uint32_t time)
{
	const int halfh = padding + (height - padding * 2) / 2;
	const int halfw = padding + (width  - padding * 2) / 2;
	int ir, or;
	uint32_t *pixel = image;
	int y;

	/* squared radii thresholds */
	or = (halfw < halfh ? halfw : halfh) - 8;
	ir = or - 32;
	or *= or;
	ir *= ir;

	pixel += padding * width;
	for (y = padding; y < height - padding; y++) {
		int x;
		int y2 = (y - halfh) * (y - halfh);

		pixel += padding;
		for (x = padding; x < width - padding; x++) {
			uint32_t v;

			/* squared distance from center */
			int r2 = (x - halfw) * (x - halfw) + y2;

			if (r2 < ir)
				v = (r2 / 32 + time / 64) * 0x0080401;
			else if (r2 < or)
				v = (y + time / 32) * 0x0080401;
			else
				v = (x + time / 16) * 0x0080401;
			v &= 0x00ffffff;

			/* cross if compositor uses X from XRGB as alpha */
			if (abs(x - y) > 6 && abs(x + y - height) > 6)
				v |= 0xff000000;

			*pixel++ = v;
		}

		pixel += padding;
	}
}

int canvas_draw(void *draw_data, void *shm_buffer)
{
	struct mycanvas_data *data = draw_data;
	uint32_t w, h;
	pilot_widget_size((struct pilot_widget *)data->window, &w, &h);
	paint_pixels(shm_buffer, 20, w, h, data->change);
	return 0;
}

int
mainwindow_init(struct pilot_window *mainwindow)
{
	struct pilot_canvas *canvas;
	
	canvas = pilot_canvas_create((struct pilot_widget *)mainwindow, WL_SHM_FORMAT_XRGB8888);
	if (!canvas)
		return -1;
	canvas_data.canvas = canvas;
	canvas_data.window = mainwindow;
	canvas_data.change = 0;

	pilot_canvas_set_draw_handler(canvas, canvas_draw, &canvas_data);
	pilot_window_set_layout(mainwindow, (struct pilot_widget*)canvas);
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
		canvas_data.change+=16;
		pilot_widget_redraw(widget);
	}
	return 0;
}
int click(struct pilot_widget *widget, pilot_key_t key)
{
	printf("click %d\n", key);
	
	return 0;
}
int main_window_focus(struct pilot_widget *mainwindow, pilot_bool_t in)
{
	pilot_widget_grabkeys(mainwindow, in);
	if (in)
	{
		printf("%p I have the focus\n", mainwindow);
		pilot_connect(mainwindow, keyPressed, mainwindow, keypressed);
	}
	else
	{
		printf("%p I lost the focus\n", mainwindow);
		pilot_disconnect(mainwindow, keyPressed, mainwindow, keypressed);
	}
}

int main(int argc, char **argv)
{
	int ret = 0;
	struct pilot_display *display;
	struct pilot_window *mainwindow;
	struct pilot_window *mainwindow2;
	struct pilot_theme *theme = NULL;

	/**
	 * Setup
	 **/
	display = pilot_display_create();
	//theme = pilot_theme_create(display);
	
	mainwindow = pilot_window_create((struct pilot_widget *)display, "mainwindow", 250, 250, theme);
	if (!mainwindow)
		return -1;
	if (mainwindow_init(mainwindow) < 0)
		return -1;

	pilot_display_add_window(display, mainwindow);

	struct pilot_widget *mainwidget = (struct pilot_widget *)mainwindow;
	pilot_connect(mainwidget, focusChanged, mainwidget, main_window_focus);
	pilot_connect(mainwidget, clicked, mainwidget, click);
	pilot_window_show(mainwindow);

/*
	mainwindow2 = pilot_window_create((struct pilot_widget *)display, "window2", 250, 250, NULL);
	if (!mainwindow2)
		return -1;
	if (mainwindow_init(mainwindow2) < 0)
		return -1;

	pilot_display_add_window(display, mainwindow2);

	pilot_window_show(mainwindow2);
*/
	/**
	 * MainLoop
	 **/
	ret = pilot_display_mainloop(display);

	/**
	 * Cleanup
	 **/
	mainwindow_fini(mainwindow);
	pilot_window_destroy(mainwindow);
	//mainwindow_fini(mainwindow2);
	//pilot_window_destroy(mainwindow2);
	pilot_display_destroy(display);
	return ret;
}
