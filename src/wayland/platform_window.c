#include "platform_wtk.h"

static const struct wl_shell_surface_listener _st_shell_surface_listener;

static	int
_platform_window_create(struct pilot_window *window, struct pilot_display *display)
{
	struct platform_window *platform = malloc(sizeof(*platform));
	struct platform_display *pl_display = display->platform;
	
	memset(platform, 0, sizeof *platform);
	platform->callback = NULL;
	platform->surface = wl_compositor_create_surface(pl_display->compositor);
	platform->shell_surface = wl_shell_get_shell_surface(pl_display->shell,
							   platform->surface);

	if (platform->shell_surface)
		wl_shell_surface_add_listener(platform->shell_surface,
					      &_st_shell_surface_listener, window);

	if (window->name)
		wl_shell_surface_set_title(platform->shell_surface, window->name);
	wl_shell_surface_set_toplevel(platform->shell_surface);
	window->platform = platform;

	if (window->opaque) {
		struct platform_display *pl_display =
									window->common.display->platform;
		platform->region = wl_compositor_create_region(pl_display->compositor);
		wl_region_add(platform->region, 0, 0, window->fullwidth, window->fullheight);
	}

	return 0;
}

static void
_platform_window_destroy(struct pilot_window *window)
{
	struct platform_window *platform = window->platform;

	if (platform->callback)
		wl_callback_destroy(platform->callback);

	wl_shell_surface_destroy(platform->shell_surface);
	wl_surface_destroy(platform->surface);
	free(window->platform);
}

static void
_pilot_window_surface_ping_handler(void *data, struct wl_shell_surface *shell_surface,
							uint32_t serial)
{
	wl_shell_surface_pong(shell_surface, serial);
}

static void
_pilot_window_surface_configure_handler(void *data, struct wl_shell_surface *shell_surface,
		 uint32_t edges, int32_t width, int32_t height)
{
	_pilot_window_resize(data, width, height);
}

static void
_pilot_window_surface_popup_done_handler(void *data, struct wl_shell_surface *shell_surface)
{
}

static const struct wl_shell_surface_listener _st_shell_surface_listener = {
	_pilot_window_surface_ping_handler,
	_pilot_window_surface_configure_handler,
	_pilot_window_surface_popup_done_handler
};

static const struct wl_callback_listener _st_frame_listener;

static int
_platform_window_flush(struct pilot_window *window)
{
	struct platform_window *platform = window->platform;

	wl_surface_set_opaque_region(platform->surface, platform->region);
	if (platform->region) {
		wl_region_destroy(platform->region);
		platform->region = NULL;
	}

	//wl_surface_damage(platform->surface, 0, 0, window->fullwidth, window->fullheight);
	wl_surface_damage(platform->surface, window->common.region.x,
								window->common.region.y, 
								window->common.region.w,
								window->common.region.h);

	platform->callback = wl_surface_frame(platform->surface);
	wl_callback_add_listener(platform->callback, &_st_frame_listener, window);

	wl_surface_commit(platform->surface);
	return 0;
}

static void
_pilot_window_frame_handler(void *data, struct wl_callback *callback, uint32_t time)
{
	struct pilot_window *window = data;
	struct platform_window *platform = window->platform;

	if (callback != platform->callback)
		return;
	platform->callback = NULL;

	if (callback)
		wl_callback_destroy(callback);

	_pilot_window_redraw(data);
}

static const struct wl_callback_listener _st_frame_listener = {
	_pilot_window_frame_handler
};
