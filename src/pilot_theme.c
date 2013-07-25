#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pilot_wtk.h>

static int
_pilot_theme_resize_window(struct pilot_theme *theme,
						pilot_rect_t *region);

struct pilot_theme *
pilot_theme_create(struct pilot_display *display)
{
	struct pilot_theme *theme;

	theme = malloc(sizeof (*theme));
	memset(theme, 0, sizeof(*theme));
	theme->window = NULL;

	theme->bgcolor = 0x00CCCCCC;
	theme->border = 2;
	theme->changed = 1;
	return theme;
}

struct pilot_theme *
pilot_theme_attach(struct pilot_theme *theme, struct pilot_window *window)
{
	struct pilot_theme *newtheme;

	newtheme = pilot_theme_create(NULL);
	newtheme->window = window;
	newtheme->bgcolor = theme->bgcolor;
	newtheme->bgcolor |= (window->opaque)? 0xFF000000: 0;
	newtheme->border = theme->border;
	
	if (newtheme->window)
	{
		int i = 0;
		pilot_rect_t region;
		newtheme->buffer = pilot_buffer_create((struct pilot_widget *)window,
									window->fullwidth,
									window->fullheight,
									window->common.format);
		region.x = window->common.region.x;
		region.y = window->common.region.y;
		region.w = window->fullwidth;
		region.h = window->fullheight;
		if (!_pilot_theme_resize_window(newtheme, &region)) {
			pilot_rect_copy(&window->common.region, &region);
		}
		pilot_buffer_fill(newtheme->buffer, newtheme->bgcolor);
		LOG_DEBUG("");
	}

	return newtheme;
}

void
pilot_theme_destroy(struct pilot_theme *theme)
{
	if (theme->buffer)
		pilot_buffer_destroy(theme->buffer);
	free(theme);
}

struct pilot_widget *
pilot_theme_get_caption(struct pilot_theme *theme)
{
	return theme->caption;
}

uint32_t
pilot_theme_get_border(struct pilot_theme *theme)
{
	return theme->border;
}

int
pilot_theme_redraw_window(struct pilot_theme *theme)
{
	int ret = 0;
	if (!theme->window)
		return 0;
	if (theme->caption && theme->changed)
		ret = pilot_widget_redraw(theme->caption);
	if (theme->buffer && theme->changed)
	{
		ret +=1;
		LOG_DEBUG("");
		pilot_buffer_paint_window(theme->buffer, theme->window);
	}
	theme->changed = 0;
	return ret;
}

static int
_pilot_theme_resize_window(struct pilot_theme *theme,
						pilot_rect_t *region)
{
	pilot_length_t decrement_height;
	if (region->w > theme->border * 2) {
		region->w -= theme->border * 2;
		region->x += theme->border;
	} else
		return -1;
	decrement_height = theme->border;
	if (theme->caption)
		decrement_height += theme->caption->region.h;
	if (region->h > decrement_height + theme->border) {
		region->h -= decrement_height + theme->border;
		region->y += decrement_height;
	} else
		return -1;

	return 0;
}
