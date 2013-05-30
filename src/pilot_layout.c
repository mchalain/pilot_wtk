#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pilot_wtk.h>

static void
_pilot_layout_destroy(void *widget);
static int
_pilot_layout_redraw(void *widget);
static int
_pilot_layout_resize(void *widget, uint32_t width, uint32_t height);

struct pilot_layout *
pilot_layout_create(struct pilot_widget *parent)
{
	struct pilot_layout *layout;
	int i;

	layout = malloc(sizeof *layout);
	memset(layout, 0, sizeof(*layout));
	pilot_widget_init(&layout->common, parent);
	pilot_widget_resize(&layout->common, parent->width, parent->height);
	layout->common.action.redraw = _pilot_layout_redraw;
	layout->common.action.resize = _pilot_layout_resize;
	layout->common.action.destroy = _pilot_layout_destroy;

	return layout;
}

static void
_pilot_layout_destroy(void *widget)
{
	struct pilot_layout *layout = widget;
	pilot_layout_destroy(layout);
}

void
pilot_layout_destroy(struct pilot_layout *layout)
{
	int i;
	
	for (i = 0; i < layout->maxwidgets; i++) {
		struct pilot_widget *widget = layout->widgets[i];
		if (!widget) break;
		pilot_widget_destroy(widget);
	}
	free(layout);
}

int
pilot_layout_add_widget(struct pilot_layout *layout, struct pilot_widget *widget)
{
	int i;

	for (i = 0; i < layout->maxwidgets; i++)
		if (!layout->widgets[i]) break;
	if (i < layout->maxwidgets)
		layout->widgets[i] = widget;
	else
		return -1;
	return 0;
}

static int
_pilot_layout_redraw(void *widget)
{
	struct pilot_layout *layout = widget;
	int i;
	
	for (i = 0; i < layout->maxwidgets; i++) {
		struct pilot_widget *widget = layout->widgets[i];
		if (!widget) break;
		pilot_widget_redraw(widget);
	}
	return 0;
}

static int
_pilot_layout_resize(void *widget, uint32_t width, uint32_t height)
{
	struct pilot_layout *layout = widget;
	int i;
	
	for (i = 0; i < layout->maxwidgets; i++) {
		struct pilot_widget *widget = layout->widgets[i];
		if (!widget) break;
		pilot_widget_resize(widget, width, height);
	}
	return 0;
}
