#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "platform_wtk.h"

static void *
_platform_display_create(struct pilot_display *display,
					struct pilot_connector *connector)
{
	int fd = -1;
	PILOT_CREATE_THIZ(platform_display);
	char *fbname = NULL;

	fbname = getenv("PILOT_FRAMEBUFFER");
	if (fbname)
		fd = open(fbname, O_RDWR | O_CLOEXEC);
	if (fd < 0)
	{
		LOG_ERROR("framebuffer erro : %s", strerror(errno));
		free(thiz);
		thiz = NULL;
	}
	else
	{
		connector->fd = fd;
	}
	return thiz;
}

static void
_platform_display_destroy(struct pilot_display *display)
{
	struct platform_display *platform = display->platform;

	free(platform);
}

static int
_platform_display_prepare_wait(struct pilot_display *display)
{
	struct platform_display *platform = display->platform;
	return 0;
}

static int
_platform_display_dispatch_events(struct pilot_display *display)
{
	struct platform_display *platform = display->platform;
	return 0;
}

inline struct  platform_display *
_platform_display(struct  pilot_display *display)
{
	return display->platform;
}
