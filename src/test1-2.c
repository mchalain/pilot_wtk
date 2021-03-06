#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <pilot_atk.h>
#include <pilot_utk.h>
#include <pilot_wtk.h>
#include <linux/input.h>

struct pilot_application *g_application;

struct mycanvas_data
{
	struct pilot_widget *canvas;
	struct pilot_window *window;
	int change;
	int incr;
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

int canvas_draw(void *draw_data, struct pilot_blit *blit)
{
	struct mycanvas_data *data = draw_data;
	uint32_t w, h;
	LOG_DEBUG("");
	data->change += data->incr;
	paint_pixels(blit->data, 20, blit->rect.w, blit->rect.h, data->change);
	// we return 0 to not force the redraw at the next frame return
	// thread_run does the redraw instead
	return 0;
}

int
canvas_key(void *draw_data, pilot_bitsfield_t key, char state)
{
	struct mycanvas_data *data = draw_data;
	if (key == 0x67)
		data->incr ++;
	else if (key == 0x6C)
		data->incr --;
	fprintf(stdout, "%X\n", key);
	return 0;
}

int
mainwindow_init(struct pilot_window *mainwindow)
{
	struct pilot_widget *canvas;
	
	canvas = pilot_canvas_create((struct pilot_widget *)mainwindow);
	if (!canvas)
		return -1;
	canvas_data.canvas = canvas;
	canvas_data.window = mainwindow;
	canvas_data.change = 0;
	canvas_data.incr = 16;

	pilot_connect(canvas, keyChanged, &canvas_data, canvas_key);
	pilot_canvas_set_draw_handler(canvas, canvas_draw, &canvas_data);
	return 0;
}

static void *
thread_run(void *data)
{
	struct pilot_window *mainwindow = (struct pilot_window *)data;
	while(1)
	{
		pilot_window_redraw(mainwindow);
	}
	return 0;
}

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

	mainwindow = pilot_window_create(display, "mainwindow", rect);
	if (!mainwindow)
		return -1;
	if (mainwindow_init(mainwindow) < 0)
		return -1;

	pthread_t thread_id;
	pthread_create(&thread_id, NULL, &thread_run, mainwindow);
	/**
	 * MainLoop
	 **/
	ret = pilot_application_run(g_application);
	LOG_DEBUG("ret %d", ret);

	/**
	 * Cleanup
	 **/
	pilot_window_destroy(mainwindow);
	pilot_display_destroy(display);
	pilot_application_destroy(g_application);
	return ret;
}
