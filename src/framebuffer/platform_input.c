#include "platform_wtk.h"

static void *
_platform_inputkeyboard_create(struct pilot_input *input, struct pilot_display *display)
{
	return NULL;
}
static void *
_platform_inputpointer_create(struct pilot_input *input, struct pilot_display *display)
{
	return NULL;
}
static void
_platform_input_destroy(struct pilot_input *thiz)
{
	struct platform_input *platform = thiz->platform;
	if (platform)
		free(platform);
}

