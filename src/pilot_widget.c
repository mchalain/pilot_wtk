#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <pilot_wtk.h>

static int
_pilot_widget_installgrab(struct pilot_widget *widget, struct pilot_input *input);

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
	if (parent) {
		widget->window = parent->window;
		widget->display = parent->display;
		pilot_rect_copy(&widget->region, &parent->region);
	}
	widget->parent = parent;
	return 0;
}

int
pilot_widget_size(struct pilot_widget *widget, uint32_t *width, uint32_t *height)
{
	*width = widget->region.w;
	*height = widget->region.h;
	return 0;
}

int
pilot_widget_show(struct pilot_widget *widget)
{
	int ret = 0;
	if (widget->action.show)
		ret = widget->action.show((void *)widget);
	return ret;
}

int
pilot_widget_redraw(struct pilot_widget *widget)
{
	int ret = 0;
	if (widget->action.redraw) {
		ret = widget->action.redraw((void *)widget);
		LOG_DEBUG("ret %d", ret);
	}
	return ret;
}

int
pilot_widget_resize(struct pilot_widget *widget, pilot_length_t width, pilot_length_t height)
{
	int ret = 0;
	if (widget->action.resize) {
		ret = widget->action.resize((void *)widget, width, height);
		LOG_DEBUG("ret %d", ret);
	}
	return ret;
}

static int
_pilot_widget_keysend(struct pilot_widget *widget, pilot_key_t key, pilot_bool_t state)
{
	LOG_DEBUG("");
	if (state)
		widget->key = key;
	else {
		if (widget->key == key)
			pilot_emit(widget,keyPressed, key);
		widget->key = 0;
	}
	return 0;
}

static int
_pilot_widget_installgrab(struct pilot_widget *widget, struct pilot_input *input)
{
	LOG_DEBUG("%p %d", input, input->id);
	if (input && input->id == PILOT_INPUT_KEYBOARD) {
		LOG_DEBUG("connect keyChanged %p => %p", input, widget);
		pilot_connect(input, keyChanged, widget, _pilot_widget_keysend);
	}
	return 0;
}

static int
_pilot_widget_uninstallgrab(struct pilot_widget *widget, struct pilot_input *input)
{
	LOG_DEBUG("%p %d", input, input->id);
	if (input && input->id == PILOT_INPUT_KEYBOARD)
		pilot_disconnect(input, keyChanged, widget, _pilot_widget_keysend);
	return 0;
}

int
pilot_widget_grabkeys(struct pilot_widget *widget, pilot_bool_t yes)
{
	if (yes) {
		pilot_list_foreach(widget->display->inputs, _pilot_widget_installgrab, widget);
		LOG_DEBUG("connect inputChanged ");
		pilot_connect(widget->display, inputChanged, widget, _pilot_widget_installgrab);
	} else {
		LOG_DEBUG("disconnect inputChanged ");
		pilot_list_foreach(widget->display->inputs, _pilot_widget_uninstallgrab, widget);
		pilot_disconnect(widget->display, inputChanged, widget, _pilot_widget_installgrab);
	}
	return 0;
}

int
pilot_widget_change_focus(struct pilot_widget *widget, struct pilot_widget *newwidgetchanged, char in)
{
	if (widget == newwidgetchanged)
	{
		struct pilot_widget *window = (struct pilot_widget *)widget->window;
		if (!window->hasfocus && in) {
			window->hasfocus = 1;
			pilot_emit(window, focusChanged, in);
		}
		if (window->hasfocus && !in) {
			window->hasfocus = 0;
			pilot_emit(window, focusChanged, in);
		}
		if (widget->hasfocus != in) {
			widget->hasfocus = in;
			pilot_emit(widget, focusChanged, in);
		}
	}
	if ((struct pilot_display *)widget == newwidgetchanged->display) {
		struct pilot_display *display = (struct pilot_display *)widget;
		struct pilot_window *focus_widget;
		pilot_bool_t (*hasfocus)(struct pilot_window *) = 
				(pilot_bool_t (*)(struct pilot_window *))pilot_widget_hasfocus;
		focus_widget = pilot_display_search_window(display, hasfocus);
		pilot_emit(display, focusChanged, (struct pilot_widget *)focus_widget, 0);
		pilot_emit(display, focusChanged, widget, 1);
	}
	return 0;
}

void
pilot_widget_focus(struct pilot_widget *widget)
{
	pilot_widget_change_focus((struct pilot_widget *)widget->display, widget, 1);
}

#ifndef HAVE_INLINE
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
#endif
