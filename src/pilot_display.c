#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <pilot_wtk.h>
#include <pilot_atk.h>

#include "pilot_wtk_internal.h"

static int running = 0;
static void *
_platform_display_create(struct pilot_display *display,
							struct pilot_connector *connector);
static void
_platform_display_destroy(struct pilot_display *display);
static int
_platform_display_prepare_wait(struct pilot_display *display);
static int
_platform_display_dispatch_events(struct pilot_display *display);
int
_platform_display_region(struct pilot_display *display,
						pilot_rect_t *region);

static int
_pilot_display_prepare_wait(struct pilot_display *thiz);
static int
_pilot_display_dispatch_events(struct pilot_display *thiz);
static int
_pilot_display_synch(struct pilot_display *thiz);

/**
 * display object
 **/
struct pilot_display *
pilot_display_create(struct pilot_application *application)
{
	struct pilot_display *thiz;
	thiz = calloc(1, sizeof(*thiz));
	if (!thiz)
		return NULL;
	memset(thiz, 0, sizeof(*thiz));

	thiz->formats = 0;

	thiz->connector = pilot_connector_create(application);
	thiz->platform = _platform_display_create(thiz, thiz->connector);
	if (thiz->platform == NULL) {
		LOG_ERROR("platform not available");
		free(thiz);
		return NULL;
	}
	pilot_connect(thiz->connector, prepare_wait, thiz, _pilot_display_prepare_wait);
	pilot_connect(thiz->connector, dispatch_events, thiz, _pilot_display_dispatch_events);

	thiz->synchconnector = pilot_connector_create(application);
	pipe(thiz->synchfd);
	thiz->synchconnector->fd = thiz->synchfd[0];
	pilot_connect(thiz->synchconnector, dispatch_events, thiz, _pilot_display_synch);

	return thiz;
}

void
pilot_display_destroy(struct pilot_display *thiz)
{
	pilot_connector_destroy(thiz->connector);
	_platform_display_destroy(thiz);
	free(thiz);
}

static int
_pilot_display_prepare_wait(struct pilot_display *thiz)
{
	int ret = 0;
	ret = _platform_display_prepare_wait(thiz);
	if (thiz->force_redraw)
		ret = 1;
	return ret;
}

static int
_pilot_display_dispatch_events(struct pilot_display *thiz)
{
	int ret = 0;
	ret = _platform_display_dispatch_events(thiz);
	return ret;
}

pilot_pixel_format_t
pilot_display_format(struct pilot_display *thiz)
{
	if (thiz->formats & PILOT_DISPLAY_ARGB8888) {
		return PILOT_DISPLAY_ARGB8888;
	}
	return PILOT_DISPLAY_XRGB8888;
}

struct pilot_display_synch
{
	char msg;
};

int
pilot_display_synch(struct pilot_display *thiz)
{
	struct pilot_display_synch synch = {.msg = 'S',};
	
	write(thiz->synchfd[1], &synch, sizeof(synch));
	LOG_DEBUG("write");
	return 0;
}

static int
_pilot_display_synch(struct pilot_display *thiz)
{
	struct pilot_display_synch synch;
	int ret;
	ret = read(thiz->synchfd[0], &synch, sizeof(synch));
	pilot_emit(thiz, synch);
	return 0;
}

#include "platform_display.c"
