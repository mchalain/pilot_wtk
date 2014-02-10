#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pilot_wtk.h>
#include <pilot_atk.h>
#include <pilot_utk.h>

/**
 * internal object pilot_canvas_data
 */
struct pilot_canvas_data
{
	f_draw_handler handler;
	void *data;
};

struct pilot_canvas_data *
pilot_canvas_data_create(f_draw_handler handler, void *data);
void
pilot_canvas_data_destroy(struct pilot_canvas_data *thiz);
/*******************************************************************/
static void
_pilot_canvas_destroy(struct pilot_widget *thiz);
static int
_pilot_canvas_redraw(struct pilot_widget *thiz, struct pilot_blit *blit);

struct pilot_widget *
pilot_canvas_create(struct pilot_widget *parent)
{
	struct pilot_widget *thiz;
	int i;

	thiz = pilot_widget_create(parent, parent->drawingrect);
	thiz->action.redraw = _pilot_canvas_redraw;
	thiz->action.destroy = _pilot_canvas_destroy;
	thiz->focussable = 1;

	return thiz;
}

static void
_pilot_canvas_destroy(struct pilot_widget *thiz)
{
	pilot_canvas_set_draw_handler(thiz, NULL, NULL);
}

static int
_pilot_canvas_redraw(struct pilot_widget *thiz, struct pilot_blit *blit)
{
	struct pilot_canvas_data *canvas = (struct pilot_canvas_data *)thiz->widget;
	int ret = 0;
	if (canvas && canvas->handler)
		ret = canvas->handler(canvas->data, blit);
	return ret;
}

int
pilot_canvas_set_draw_handler(struct pilot_widget *thiz, f_draw_handler handler, void *data)
{
	int ret = -1;
	
	if (thiz->widget)
	{
		pilot_canvas_data_destroy((struct pilot_canvas_data *)thiz->widget);
	}
	if (handler)
	{
		thiz->widget = (void*)pilot_canvas_data_create(handler,data);
		ret = 0;
	}
	return ret;
}	

void *
pilot_canvas_draw_data(struct pilot_widget *thiz)
{
	struct pilot_canvas_data *canvas = (struct pilot_canvas_data *)thiz->widget;
	return canvas->data;
}

struct pilot_canvas_data *
pilot_canvas_data_create(f_draw_handler handler, void *data)
{
	PILOT_CREATE_THIZ(pilot_canvas_data);

	thiz->handler = handler;
	thiz->data = data;
	return thiz;
}

void
pilot_canvas_data_destroy(struct pilot_canvas_data *thiz)
{
	free(thiz);
}
