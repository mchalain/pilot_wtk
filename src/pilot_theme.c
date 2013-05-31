#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pilot_wtk.h>

struct pilot_theme *
pilot_theme_create(struct pilot_window *window)
{
	struct pilot_theme *theme;

	theme = malloc(sizeof (*theme));
	memset(theme, 0, sizeof(*theme));

	return theme;
}

void
pilot_theme_destroy(struct pilot_theme *theme)
{
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
