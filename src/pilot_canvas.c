#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pilot_wtk.h>

static int
_pilot_canvas_add_buffer(struct pilot_canvas *canvas);
static struct pilot_buffer *
_pilot_canvas_next_buffer(struct pilot_canvas *canvas);
static void
_pilot_canvas_destroy(void *widget);
static int
_pilot_canvas_redraw(void *widget);
static int
_pilot_canvas_resize(void *widget, uint32_t width, uint32_t height);

struct pilot_canvas *
pilot_canvas_create(struct pilot_widget *parent, int format)
{
	struct pilot_canvas *canvas;
	int i;

	canvas = malloc(sizeof *canvas);
	memset(canvas, 0, sizeof(*canvas));
	pilot_widget_init(&canvas->common, parent);
	canvas->common.width = parent->width;
	canvas->common.height = parent->height;
	canvas->common.action.redraw = _pilot_canvas_redraw;
	canvas->common.action.resize = _pilot_canvas_resize;
	canvas->common.action.destroy = _pilot_canvas_destroy;

	canvas->common.format = format;

	for (i = 0; i < 2; i++)
		if (_pilot_canvas_add_buffer(canvas) < 0) {
			free(canvas);
			return NULL;
		}
		
	return canvas;
}

static void
_pilot_canvas_destroy(void *widget)
{
	struct pilot_canvas *canvas = widget;
	pilot_canvas_destroy(canvas);
}

void
pilot_canvas_destroy(struct pilot_canvas *canvas)
{
	int i;
	for (i = 0; i < MAXBUFFERS; i++)
		if (canvas->buffers[i])
			pilot_buffer_destroy(canvas->buffers[i]);

	free(canvas);
}

int
pilot_canvas_set_draw_handler(struct pilot_canvas *canvas, f_draw_handler handler, void *data)
{
	int ret = -1;
	if (!canvas->draw_handler) {
		canvas->draw_handler = handler;
		canvas->draw_data = data;
		ret = 0;
	}
	return ret;
}	

void *
pilot_canvas_draw_data(struct pilot_canvas *canvas)
{
	return canvas->draw_data;
}

static int
_pilot_canvas_redraw(void *widget)
{
	struct pilot_canvas *canvas = widget;
	struct pilot_buffer *buffer;
	struct pilot_window *window = (struct pilot_window *)canvas->common.window;

	buffer = _pilot_canvas_next_buffer(canvas);
	if (!buffer) {
		fprintf(stderr,
			"Both buffers busy at redraw(). Server bug?\n");
		abort();
	}
	if (canvas->draw_handler)
		canvas->draw_handler(canvas->draw_data, buffer->shm_data);

	pilot_buffer_paint_window(buffer, window);
	return 0;
}

static int
_pilot_canvas_resize(void *widget, uint32_t width, uint32_t height)
{
	struct pilot_canvas *canvas = widget;
	return 0;
}

static int
_pilot_canvas_add_buffer(struct pilot_canvas *canvas)
{
	struct pilot_buffer *buffer;
	int i;

	for (i = 0; i< MAXBUFFERS; i++) if (canvas->buffers[i] == NULL) break;
	if (i == MAXBUFFERS)
		return -1;
	buffer = pilot_buffer_create((struct pilot_widget *)canvas);

	if (!buffer)
		return -1;
	canvas->buffers[i] = buffer;

	return 0;
}

static struct pilot_buffer *
_pilot_canvas_next_buffer(struct pilot_canvas *canvas)
{
	struct pilot_buffer *buffer = NULL;
	int i = 0;

	for (i = 0; i < MAXBUFFERS && canvas->buffers[i]; i++)
		if (!pilot_buffer_busy(canvas->buffers[i])) {
			buffer = canvas->buffers[i];
			break;
		}
	if (buffer)
		canvas->next_buffer = buffer;
	return buffer;
}

