#ifdef HAVE_POINTER
#include "platform_display_pointer.c"
#endif
#ifdef HAVE_KEYBOARD
#include "platform_display_keyboard.c"
#endif

static void
_seat_handle_capabilities(void *data, struct wl_seat *seat,
			 enum wl_seat_capability caps)
{
	struct pilot_display *d = data;

#ifdef HAVE_POINTER
	if ((caps & WL_SEAT_CAPABILITY_POINTER) && !d->platform.pointer) {
		d->platform.pointer = wl_seat_get_pointer(seat);
		wl_pointer_add_listener(d->platform.pointer, &_st_pointer_listener, d);
	} else if (!(caps & WL_SEAT_CAPABILITY_POINTER) && d->platform.pointer) {
		wl_pointer_destroy(d->platform.pointer);
		d->platform.pointer = NULL;
	}
#endif
#ifdef HAVE_KEYBOARD
	if ((caps & WL_SEAT_CAPABILITY_KEYBOARD) && !d->platform.keyboard) {
		d->platform.keyboard = wl_seat_get_keyboard(seat);
		wl_keyboard_add_listener(d->platform.keyboard, &_st_keyboard_listener, d);
	} else if (!(caps & WL_SEAT_CAPABILITY_KEYBOARD) && d->platform.keyboard) {
		wl_keyboard_destroy(d->platform.keyboard);
		d->platform.keyboard = NULL;
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

	if (strcmp(interface, "wl_compositor") == 0) {
		d->platform.compositor = wl_registry_bind(registry,
					 id, &wl_compositor_interface, 1);
	} else if (strcmp(interface, "wl_shell") == 0) {
		d->platform.shell = wl_registry_bind(registry,
					    id, &wl_shell_interface, 1);
	} else if (strcmp(interface, "wl_seat") == 0) {
		d->platform.seat = wl_registry_bind(registry,
					    id, &wl_seat_interface, 1);
		wl_seat_add_listener(d->platform.seat, &_st_seat_listener, d);
	} else if (strcmp(interface, "wl_shm") == 0) {
		d->platform.shm = wl_registry_bind(registry,
					  id, &wl_shm_interface, 1);
		wl_shm_add_listener(d->platform.shm, &_st_shm_listenter, d);
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
_platform_display_create(struct pilot_display *display)
{
	display->platform.display = wl_display_connect(NULL);
	if (!display->platform.display) {
		return -1;
	}

	display->platform.registry = wl_display_get_registry(display->platform.display);
	wl_registry_add_listener(display->platform.registry,
				 &_st_registry_listener, display);
	wl_display_roundtrip(display->platform.display);

	if (display->platform.shm == NULL) {
		fprintf(stderr, "No wl_shm global\n");
		return -1;
	}

	wl_display_roundtrip(display->platform.display);

	if (!(display->formats & (1 << WL_SHM_FORMAT_XRGB8888))) {
		fprintf(stderr, "WL_SHM_FORMAT_XRGB32 not available\n");
		return -1;
	}

	wl_display_get_fd(display->platform.display);

	return 0;
}

static void
_platform_display_destroy(struct pilot_display *display)
{
	if (display->platform.shm)
		wl_shm_destroy(display->platform.shm);

	if (display->platform.shell)
		wl_shell_destroy(display->platform.shell);

	if (display->platform.compositor)
		wl_compositor_destroy(display->platform.compositor);

	wl_registry_destroy(display->platform.registry);
	wl_display_flush(display->platform.display);
	wl_display_disconnect(display->platform.display);
}

static int
_platform_display_dispatch_events(struct pilot_display *display)
{
	return wl_display_dispatch(display->platform.display);
}
