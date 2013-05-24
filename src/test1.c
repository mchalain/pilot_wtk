#include <pilot_wtk.h>

struct mycanvas_data
{
	struct pilot_canvas *canvas;
	struct pilot_window *window;
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
	paint_pixels(shm_buffer, 20, data->window->width, data->window->height, 0);
	return 0;
}

int
mainwindow_init(struct pilot_window *mainwindow)
{
	struct pilot_canvas *canvas;
	
	canvas = pilot_canvas_create(mainwindow, WL_SHM_FORMAT_XRGB8888);
	canvas_data.canvas = canvas;
	canvas_data.window = mainwindow;

	pilot_canvas_set_draw_handler(canvas, canvas_draw, &canvas_data);
	pilot_window_set_canvas(mainwindow, canvas);
}

void
mainwindow_fini(struct pilot_window *mainwindow)
{
	pilot_window_set_canvas(mainwindow, NULL);
	pilot_canvas_destroy(mainwindow->canvas);
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

	mainwindow = pilot_window_create(display, argv[0], 250, 250);
	mainwindow_init(mainwindow);

	pilot_display_add_window(display, mainwindow);

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
