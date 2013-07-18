#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pilot_wtk.h>

struct pilot_theme *
pilot_theme_create(struct pilot_display *display)
{
	struct pilot_theme *theme;

	theme = malloc(sizeof (*theme));
	memset(theme, 0, sizeof(*theme));
	theme->window = NULL;

	theme->bgcolor = 0x0055A5AA;
	theme->border = 2;
	return theme;
}

struct pilot_theme *
pilot_theme_duplicate(struct pilot_theme *theme)
{
	struct pilot_theme *newtheme;

	newtheme = pilot_theme_create(NULL);
	newtheme->window = theme->window;
	newtheme->bgcolor = theme->bgcolor | (newtheme->window->opaque)? 0xFF000000: 0;
	newtheme->border = theme->border;
	
	if (newtheme->window)
	{
		int i = 0;
		newtheme->buffer = pilot_buffer_create((struct pilot_widget *)newtheme->window);
		pilot_buffer_draw(newtheme->buffer, theme->bgcolor);
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
	if (theme->caption)
		ret = pilot_widget_redraw(theme->caption);
	if (theme->buffer)
	{
		ret +=1;
		pilot_buffer_paint_window(theme->buffer, theme->window->common.window);
	}
	return ret;
}

int
pilot_theme_resize_window(struct pilot_theme *theme, pilot_length_t *width, pilot_length_t *height)
{
	pilot_length_t decrement_height;
	if (*width > theme->border * 2)
		*width -= theme->border * 2;
	else
		return -1;
	decrement_height = theme->border * 2;
	if (theme->caption)
		decrement_height += theme->caption->height;
	if (*height > decrement_height)
		*height -= decrement_height;
	else
		return -1;
	return 0;
}
