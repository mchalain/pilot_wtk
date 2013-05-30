#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <pilot_wtk.h>

struct pilot_widget *
pilot_widget_create(struct pilot_widget *parent)
{
	struct pilot_widget *widget;

	widget = malloc(sizeof *widget);
	memset(widget, 0, sizeof(*widget));

	pilot_widget_init(widget, parent);
	return widget;
}
void
pilot_widget_destroy(struct pilot_widget *widget)
{
	if (widget->action.destroy)
		widget->action.destroy((void *)widget);
	else
		free(widget);
}

int
pilot_widget_init(struct pilot_widget *widget, struct pilot_widget *parent)
{
	if (!parent) {
		widget->display = (struct pilot_display*)widget;
		widget->is_display = 1;
	} else {
		widget->display = parent->display;
		widget->is_display = 0;
	}
	widget->parent = parent;
	return 0;
}

int
pilot_widget_resize(struct pilot_widget *widget, uint32_t width, uint32_t height)
{
	widget->width = width;
	widget->height = height;
	return 0;
}

int
pilot_widget_size(struct pilot_widget *widget, uint32_t *width, uint32_t *height)
{
	*width = widget->width;
	*height = widget->height;
	return 0;
}

int
pilot_widget_redraw(struct pilot_widget *widget)
{
	if (widget->action.redraw)
		widget->action.redraw((void *)widget);
	return 0;
}

static int
_pilot_widget_keysend(struct pilot_widget *widget, pilot_key_t key, pilot_bool_t state)
{
	if (state)
		widget->key = key;
	else {
		if (widget->key == key)
			pilot_emit(widget,keyPressed, key);
		widget->key = 0;
	}
	return 0;
}

int
pilot_widget_grabkeys(struct pilot_widget *widget, pilot_bool_t yes)
{
	if (yes)
		pilot_connect(widget->display, keyChanged, widget, _pilot_widget_keysend);
	else
		pilot_disconnect(widget->display, keyChanged, widget, _pilot_widget_keysend);
	return 0;
}

void
pilot_widget_focus(struct pilot_widget *widget)
{
	pilot_emit(widget->parent, focusChanged, 0);
	pilot_emit_to(widget->parent, focusChanged, widget, 0);
}

pilot_bool_t
pilot_widget_hasfocus(struct pilot_widget *widget)
{
	return widget->hasfocus;
}

pilot_bool_t
pilot_widget_hasmouse(struct pilot_widget *widget)
{
	return widget->hasmouse;
}

struct pilot_display *
pilot_widget_display(struct pilot_widget *widget)
{
	return widget->display;
}
