#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pilot_wtk.h>

static char _pilot_layout_columns[] =
{
	1, 1, 1, 2, 2, 2, 2, 3, 3, 3
};
static char _pilot_layout_lines[] =
{
	1, 1, 2, 2, 2, 3, 3, 3, 3, 3
};
static void
_pilot_layout_destroy(void *widget);
static int
_pilot_layout_redraw(void *widget);
static int
_pilot_layout_resize(void *widget, pilot_length_t width, pilot_length_t height);

struct pilot_layout *
pilot_layout_create(struct pilot_widget *parent)
{
	struct pilot_layout *layout;
	int i;

	layout = malloc(sizeof *layout);
	memset(layout, 0, sizeof(*layout));
	pilot_widget_init(&layout->common, parent);
	layout->common.action.redraw = _pilot_layout_redraw;
	layout->common.action.resize = _pilot_layout_resize;
	layout->common.action.destroy = _pilot_layout_destroy;
	layout->maxwidgets = MAXWIDGETS;

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
	{
		if (widget)
		{
			layout->widgets[i] = widget;
		}
		layout->nbwidgets++;
	}
	else
		return -1;
	return 0;
}

static int
_pilot_layout_redraw(void *widget)
{
	struct pilot_layout *layout = widget;
	int i;
	
	for (i = 0; i < layout->nbwidgets; i++) {
		struct pilot_widget *widget = layout->widgets[i];
		if (!widget) continue;
		pilot_widget_redraw(widget);
	}
	return 0;
}

static int
_pilot_layout_resize(void *widget, pilot_length_t width, pilot_length_t height)
{
	struct pilot_layout *layout = widget;
	int i;
	pilot_length_t widget_width = 0, widget_height = 0;
	
	for (i = 0; i < layout->nbwidgets; i++) {
		struct pilot_widget *widget = layout->widgets[i];
		widget_width += width / _pilot_layout_columns[layout->nbwidgets];
		widget_height += height / _pilot_layout_lines[layout->nbwidgets];
		if (!widget) {
			widget_height = 0;
			if ( i % _pilot_layout_columns[layout->nbwidgets] == _pilot_layout_columns[layout->nbwidgets])
				widget_width = 0;
			continue;
		}
		widget->action.resize(widget, widget_width, widget_height);
		widget_width = 0, widget_height = 0;
	}
	return 0;
}
