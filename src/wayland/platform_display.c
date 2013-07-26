#include "platform_wtk.h"

static void
_seat_handle_capabilities(void *data, struct wl_seat *seat,
			 enum wl_seat_capability caps)
{
	struct pilot_display *display = data;
	struct platform_display *platform = display->platform;

#ifdef HAVE_POINTER
	if ((caps & WL_SEAT_CAPABILITY_POINTER)) {
		platform->seat = seat;
		struct pilot_input * input = pilot_input_create(display,PILOT_INPUT_POINTER);
		pilot_display_add_input(display, input);
	} else if (!(caps & WL_SEAT_CAPABILITY_POINTER)) {
		struct pilot_input * input = NULL;
		//pilot_input_destroy(input);
		pilot_emit(display, inputChanged, input);
	}
#endif
#ifdef HAVE_KEYBOARD
	if ((caps & WL_SEAT_CAPABILITY_KEYBOARD)) {
		platform->seat = seat;
		struct pilot_input * input = pilot_input_create(display,PILOT_INPUT_KEYBOARD);
		pilot_display_add_input(display, input);
	} else if (!(caps & WL_SEAT_CAPABILITY_KEYBOARD)) {
		struct pilot_input * input = NULL;
		//pilot_input_destroy(input);
		pilot_emit(display, inputChanged, input);
	}
#endif
}

static const struct wl_seat_listener _st_seat_listener = {
	_seat_handle_capabilities,
};

static void
_shm_format(void *data, struct wl_shm *wl_shm, uint32_t format)
{
	struct pilot_display *d = data;

	d->formats |= (1 << format);
}

struct wl_shm_listener _st_shm_listenter = {
	_shm_format
};

static void
_registry_handle_global(void *data, struct wl_registry *registry,
		       uint32_t id, const char *interface, uint32_t version)
{
	struct pilot_display *d = data;
	struct platform_display *platform = d->platform;

	if (strcmp(interface, "wl_compositor") == 0) {
		platform->compositor = wl_registry_bind(registry,
					 id, &wl_compositor_interface, 1);
	} else if (strcmp(interface, "wl_shell") == 0) {
		platform->shell = wl_registry_bind(registry,
					    id, &wl_shell_interface, 1);
	} else if (strcmp(interface, "wl_seat") == 0) {
		platform->seat = wl_registry_bind(registry,
					    id, &wl_seat_interface, 1);
		wl_seat_add_listener(platform->seat, &_st_seat_listener, d);
	} else if (strcmp(interface, "wl_shm") == 0) {
		platform->shm = wl_registry_bind(registry,
					  id, &wl_shm_interface, 1);
		wl_shm_add_listener(platform->shm, &_st_shm_listenter, d);
	}
}

static void
_registry_handle_global_remove(void *data, struct wl_registry *registry,
			      uint32_t name)
{
}

static const struct wl_registry_listener _st_registry_listener = {
	_registry_handle_global,
	_registry_handle_global_remove
};
static int
_platform_display_create(struct pilot_display *display,
					struct pilot_connector *connector)
{
	struct platform_display *platform;
	display->platform = malloc(sizeof(struct platform_display));
	platform = display->platform;
	
	platform->display = wl_display_connect(NULL);
	if (!platform->display) {
		LOG_ERROR("display not found");
		return -1;
	}

	platform->registry = wl_display_get_registry(platform->display);
	wl_registry_add_listener(platform->registry,
				 &_st_registry_listener, display);
	wl_display_roundtrip(platform->display);

	if (platform->shm == NULL) {
		LOG_ERROR("no wl_shm global");
		return -1;
	}

	wl_display_roundtrip(platform->display);

	if (!(display->formats & (1 << WL_SHM_FORMAT_XRGB8888))) {
		LOG_ERROR("WL_SHM_FORMAT_XRGB32 not available");
		return -1;
	}

	connector->fd = wl_display_get_fd(platform->display);

	return 0;
}

static void
_platform_display_destroy(struct pilot_display *display)
{
	struct platform_display *platform = display->platform;

	if (platform->shm)
		wl_shm_destroy(platform->shm);

	if (platform->shell)
		wl_shell_destroy(platform->shell);

	if (platform->compositor)
		wl_compositor_destroy(platform->compositor);

	wl_registry_destroy(platform->registry);
	wl_display_flush(platform->display);
	wl_display_disconnect(platform->display);
	free(platform);
}

static int
_platform_display_prepare_wait(struct pilot_display *display)
{
	struct platform_display *platform = display->platform;
	return wl_display_flush(platform->display);
}

static int
_platform_display_dispatch_events(struct pilot_display *display)
{
	struct platform_display *platform = display->platform;
	return wl_display_dispatch(platform->display);
}

struct pilot_window *
_platform_display_search_window(struct pilot_display *display, struct wl_surface *surface)
{
	typeof (display->windows) *windows_it = &display->windows;
	while (windows_it->item) {
		struct platform_window *pl_window = windows_it->item->platform;
		if (pl_window->surface == surface) break;
		windows_it = windows_it->next;
	}
	return windows_it->item;
}

int
_platform_display_region(struct pilot_display *display,
					pilot_rect_t *region)
{
	return 0;
}

static pilot_pixel_format_t
_platform_display_format(struct pilot_display *display)
{
	if (display->formats & PILOT_DISPLAY_ARGB8888) {
		return PILOT_DISPLAY_ARGB8888;
	}
	return PILOT_DISPLAY_XRGB8888;
}

