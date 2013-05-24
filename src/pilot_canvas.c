#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pilot_wtk.h>

struct pilot_canvas *
pilot_canvas_create(struct pilot_window *window, int format)
{
	struct pilot_canvas *canvas;

	canvas = malloc(sizeof *canvas);
	canvas->window = window;
	if (window)
	{
		canvas->width = window->width;
		canvas->height = window->height;
	}
	canvas->format = format;
	return canvas;
}

void
pilot_canvas_destroy(struct pilot_canvas *canvas)
{
	free(canvas);
}

int
pilot_canvas_set_draw_handler(struct pilot_canvas *canvas, f_draw_handler *handler, void *data)
{
	int ret = -1;
	if (!canvas->draw_handler) {
		canvas->draw_handler = handler;
		canvas->draw_data = data;
		ret = 0;
	}
	return ret;
}	

