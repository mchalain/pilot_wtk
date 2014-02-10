#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pilot_wtk.h>
#include <pilot_atk.h>
#include <pilot_utk.h>

#include "pilot_wtk_internal.h"

struct pilot_theme
{
	struct pilot_widget *caption;
	pilot_color_t bgcolor;
	pilot_length_t border;
	pilot_bool_t changed:1;
};

struct pilot_theme *
pilot_theme_create(struct pilot_display *display)
{
	PILOT_CREATE_THIZ(pilot_theme);

	thiz->bgcolor = 0x00CCCCCC;
	thiz->border = 20;
	thiz->changed = 1;
	return thiz;
}


void
pilot_theme_destroy(struct pilot_theme *thiz)
{
	free(thiz);
}

struct pilot_widget *
pilot_theme_get_caption(struct pilot_theme *thiz)
{
	return thiz->caption;
}

uint32_t
pilot_theme_get_border(struct pilot_theme *thiz)
{
	return thiz->border;
}

int
pilot_theme_redraw(struct pilot_theme *thiz, struct pilot_blit *blit)
{
	int ret = 0;
	if (thiz->caption && thiz->changed)
		ret = pilot_widget_redraw(thiz->caption, blit);
	thiz->changed = 0;
	return ret;
}
