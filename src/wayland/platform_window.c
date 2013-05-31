static const struct wl_shell_surface_listener _st_shell_surface_listener;

static	int
_platform_window_create(struct pilot_window *window, struct pilot_display *display)
{
	window->platform.callback = NULL;
	window->platform.surface = wl_compositor_create_surface(display->platform.compositor);
	window->platform.shell_surface = wl_shell_get_shell_surface(display->platform.shell,
							   window->platform.surface);

	if (window->platform.shell_surface)
		wl_shell_surface_add_listener(window->platform.shell_surface,
					      &_st_shell_surface_listener, window);

	if (window->name)
		wl_shell_surface_set_title(window->platform.shell_surface, window->name);
	wl_shell_surface_set_toplevel(window->platform.shell_surface);

	return 0;
}

static void
_platform_window_destroy(struct pilot_window *window)
{
	if (window->platform.callback)
		wl_callback_destroy(window->platform.callback);

	wl_shell_surface_destroy(window->platform.shell_surface);
	wl_surface_destroy(window->platform.surface);
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
	if (window->opaque || window->fullscreen) {
		struct wl_region *region;
		region = wl_compositor_create_region(window->common.display->platform.compositor);
		wl_region_add(region, 0, 0, window->fullwidth, window->fullheight);
		wl_surface_set_opaque_region(window->platform.surface, region);
		wl_region_destroy(region);
	} else {
		wl_surface_set_opaque_region(window->platform.surface, NULL);
	}

	window->platform.callback = wl_surface_frame(window->platform.surface);
	wl_callback_add_listener(window->platform.callback, &_st_frame_listener, window);
	return 0;
}

static void
_pilot_window_frame_handler(void *data, struct wl_callback *callback, uint32_t time)
{
	struct pilot_window *window = data;

	if (callback != window->platform.callback)
		return;
	window->platform.callback = NULL;

	if (callback)
		wl_callback_destroy(callback);

	_pilot_window_redraw(data);
}

static const struct wl_callback_listener _st_frame_listener = {
	_pilot_window_frame_handler
};
