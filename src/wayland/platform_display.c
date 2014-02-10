#include "platform_wtk.h"

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
		_platform_display_inputmanager(d, registry, id);
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
static void *
_platform_display_create(struct pilot_display *display,
					struct pilot_connector *connector)
{
	PILOT_CREATE_THIZ(platform_display);
	
	thiz->display = wl_display_connect(NULL);
	if (!thiz->display) {
		LOG_ERROR("display not found");
		free(thiz);
		return NULL;
	}

	// display->platform must be initialized here
	// display_roundtrip will run and require platform
	display->platform = thiz;
	thiz->registry = wl_display_get_registry(thiz->display);
	wl_registry_add_listener(thiz->registry,
				 &_st_registry_listener, display);
	wl_display_roundtrip(thiz->display);

	if (thiz->shm == NULL) {
		LOG_ERROR("no wl_shm global");
		free(thiz);
		return NULL;
	}

	wl_display_roundtrip(thiz->display);

	if (!(display->formats & (1 << WL_SHM_FORMAT_XRGB8888))) {
		LOG_ERROR("WL_SHM_FORMAT_XRGB32 not available");
		free(thiz);
		return NULL;
	}

	connector->fd = wl_display_get_fd(thiz->display);

	return thiz;
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
	wl_display_flush(platform->display);
	return 0;
}

static int
_platform_display_dispatch_events(struct pilot_display *display)
{
	struct platform_display *platform = display->platform;
	return wl_display_dispatch(platform->display);
}

inline struct  platform_display *
_platform_display(struct  pilot_display *display)
{
	return display->platform;
}

static int
_platform_window_search(struct pilot_window **out, struct pilot_window *window, struct wl_surface *search)
{
	struct platform_surface *platform = window->surface->platform;
	if (platform->surface == search)
	{
		*out = window;
		return -1;
	}
	return 0;
}

struct pilot_window *
_platform_display_search_window(struct pilot_display *display, struct wl_surface *surface)
{
	struct pilot_window *result;
	pilot_list_foreach(display->windows, _platform_window_search, &result, surface);
	
	return result;
}

