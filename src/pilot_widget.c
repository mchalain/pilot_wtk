#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <pilot_wtk.h>
#include <pilot_atk.h>

struct pilot_widget *
pilot_widget_create(struct pilot_widget *parent, struct pilot_rect rect)
{
	PILOT_CREATE_THIZ(pilot_widget);

	thiz->type = EWidgetCommon;
	thiz->parent = parent;
	pilot_list_append(parent->childs, thiz);

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
_pilot_widget_redraw_child(struct pilot_widget *thiz, struct pilot_widget *child)
{
	return pilot_widget_redraw(child);
}

int
pilot_widget_redraw(struct pilot_widget *thiz)
{
	if (thiz->action.redraw)
		thiz->action.redraw(thiz);
	pilot_list_foreach(thiz->childs, _pilot_widget_redraw_child, thiz);
	return 0;
}
