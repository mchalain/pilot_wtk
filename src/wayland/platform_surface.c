#include "platform_wtk.h"

static const struct wl_shell_surface_listener _st_shell_surface_listener;

static void *
_platform_surface_create(struct pilot_surface *surface, struct pilot_display *display)
{
	struct platform_surface *thiz;
	thiz = calloc(1, sizeof(*thiz));
	if (!thiz)
		return NULL;
	memset(thiz, 0, sizeof(*thiz));

	struct platform_display *pl_display = _platform_display(display);

	thiz->callback = NULL;
	thiz->surface = wl_compositor_create_surface(pl_display->compositor);
	thiz->shell_surface = wl_shell_get_shell_surface(pl_display->shell,
							   thiz->surface);

	if (thiz->shell_surface)
		wl_shell_surface_add_listener(thiz->shell_surface,
					      &_st_shell_surface_listener, surface);

	wl_shell_surface_set_toplevel(thiz->shell_surface);

	thiz->region = wl_compositor_create_region(pl_display->compositor);
	wl_region_add(thiz->region, 0, 0, surface->width, surface->height);

	return (void *)thiz;
}

static void
_platform_surface_destroy(struct pilot_surface *surface)
{
	struct platform_surface *platform = surface->platform;

	if (platform->callback)
		wl_callback_destroy(platform->callback);

	wl_shell_surface_destroy(platform->shell_surface);
	wl_surface_destroy(platform->surface);
	free(platform);
}

static const struct wl_callback_listener _st_frame_listener;
static int
_platform_surface_flush(struct pilot_surface *surface)
{
	struct platform_surface *platform = surface->platform;

	wl_surface_set_opaque_region(platform->surface, platform->region);
	if (platform->region) {
		wl_region_destroy(platform->region);
		platform->region = NULL;
	}

	//wl_surface_damage(platform->surface, 0, 0, window->fullwidth, window->fullheight);
	wl_surface_damage(platform->surface, 0,
								0, 
								surface->width,
								surface->height);

	platform->callback = wl_surface_frame(platform->surface);
	wl_callback_add_listener(platform->callback, &_st_frame_listener, surface);

	wl_surface_commit(platform->surface);
	return 0;
}

inline struct  platform_surface *
_platform_surface(struct  pilot_surface *surface)
{
	return surface->platform;
}

/**
 * shell_surface_listener
 */
static void
_pilot_surface_ping_handler(void *data, struct wl_shell_surface *shell_surface,
							uint32_t serial)
{
	struct pilot_surface *surface = data;
	wl_shell_surface_pong(shell_surface, serial);
}

static void
_pilot_surface_configure_handler(void *data, struct wl_shell_surface *shell_surface,
		 uint32_t edges, int32_t width, int32_t height)
{
	struct pilot_surface *surface = data;
}

static void
_pilot_surface_popup_done_handler(void *data, struct wl_shell_surface *shell_surface)
{
	struct pilot_surface *surface = data;
}

static const struct wl_shell_surface_listener _st_shell_surface_listener = {
	_pilot_surface_ping_handler,
	_pilot_surface_configure_handler,
	_pilot_surface_popup_done_handler
};
/**************************************************************************************/

/**
 * shell_surface_listener
 */
static void
_pilot_surface_frame_handler(void *data, struct wl_callback *callback, uint32_t time)
{
	struct pilot_surface *surface = data;
	struct platform_surface *platform = surface->platform;

	LOG_DEBUG("1");
	if (callback != platform->callback)
		return;
	platform->callback = NULL;

	if (callback)
		wl_callback_destroy(callback);
	surface->ready = 1;
	pilot_display_synch(surface->display);
}

static const struct wl_callback_listener _st_frame_listener = {
	_pilot_surface_frame_handler
};
