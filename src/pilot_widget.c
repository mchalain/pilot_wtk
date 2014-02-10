#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <pilot_wtk.h>
#include <pilot_atk.h>

#include "pilot_wtk_internal.h"

static int
_pilot_widget_appendchild(struct pilot_widget *thiz, struct pilot_widget *child);
static struct pilot_window *
_pilot_widget_window(struct pilot_widget *thiz);

struct pilot_widget *
pilot_widget_create(struct pilot_widget *parent, struct pilot_rect rect)
{
	PILOT_CREATE_THIZ(pilot_widget);

	thiz->type = EWidgetCommon;
	thiz->parent = parent;
	if (parent->type != EWidgetWindow)
		_pilot_widget_appendchild(parent, thiz);
	else
	{
		struct pilot_window *window = (struct pilot_window *)parent;
		window->layout = thiz;
		memcpy(&thiz->drawingrect, &parent->drawingrect, sizeof(thiz->drawingrect));
	}

	return thiz;
}

static int
_pilot_widget_destroy_child(struct pilot_widget *thiz, struct pilot_widget *child)
{
	pilot_widget_destroy(child);
	return 0;
}

void
pilot_widget_destroy(struct pilot_widget *thiz)
{
	pilot_list_foreach(thiz->childs, _pilot_widget_destroy_child, thiz);
	pilot_list_append(thiz->parent->childs, thiz);
	if (thiz->action.destroy)
		thiz->action.destroy(thiz);
	free(thiz);
}

static int
_pilot_widget_redraw_child(struct pilot_widget *thiz, struct pilot_widget *child, struct pilot_blit *blit)
{
	return pilot_widget_redraw(child, blit);
}

int
pilot_widget_redraw(struct pilot_widget *thiz, struct pilot_blit *blit)
{
	int ret = 0;
	struct pilot_window *window = _pilot_widget_window(thiz);
	if (thiz->action.redraw)
		ret = thiz->action.redraw(thiz, blit);
	if (ret)
		window->force_redraw = 1;
	pilot_list_foreach(thiz->childs, _pilot_widget_redraw_child, thiz, blit);
	return ret;
}

static int
_pilot_widget_appendchild(struct pilot_widget *thiz, struct pilot_widget *child)
{
	pilot_list_append(thiz->childs, child);
	// we have change the code, because each child overlap the previous one.
	// some widgets can manage the placement of the childs and calls another function.
	memcpy(&child->drawingrect, &thiz->drawingrect, sizeof(child->drawingrect));
	return 0;
}

static struct pilot_window *
_pilot_widget_window(struct pilot_widget *thiz)
{
	struct pilot_widget *parent = thiz;
	while(parent->type != EWidgetWindow) parent = parent->parent;
	return (struct pilot_window *)parent;
}

static int
_pilot_widget_focussable(struct pilot_widget *thiz, struct pilot_widget *child, struct pilot_widget **out)
{
	struct pilot_widget *focus = NULL;
	focus = pilot_widget_getfocus(child);
	if (focus != NULL)
	{
		*out = focus;
		return -1;
	}
	return 0;
}

struct pilot_widget *
pilot_widget_getfocus(struct pilot_widget *thiz)
{
	struct pilot_widget *focus = NULL;
	if (thiz->focussable)
		focus = thiz;
	else
	{
		pilot_list_foreach(thiz->childs, _pilot_widget_focussable, thiz, &focus);
	}
	return focus;
}

struct pilot_widget *
pilot_widget_getchildat(struct pilot_widget *thiz, pilot_coord_t x, pilot_coord_t y)
{
	//TODO
	return thiz;
}
