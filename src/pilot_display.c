#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <pilot_wtk.h>

static const struct wl_callback_listener _st_frame_listener;
static int running = 0;

static void
_pilot_display_redraw(void *data, struct wl_callback *callback, uint32_t time)
{
	struct pilot_window *windowlist = data;
	struct pilot_window *window = &windowlist[0];

	if (callback)
	{
		int i;
		for (i = 0; window->callback != callback && i < MAXWINDOWS; i++)
			window = &windowlist[i];
	}
	pilot_window_redraw(window);

	if (callback)
		wl_callback_destroy(callback);

	window->callback = wl_surface_frame(window->surface);
	wl_callback_add_listener(window->callback, &_st_frame_listener, windowlist);
	wl_surface_commit(window->surface);
}

static const struct wl_callback_listener _st_frame_listener = {
	_pilot_display_redraw
};

/**
 * display object
 **/
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
		d->compositor =
			wl_registry_bind(registry,
					 id, &wl_compositor_interface, 1);
	} else if (strcmp(interface, "wl_shell") == 0) {
		d->shell = wl_registry_bind(registry,
					    id, &wl_shell_interface, 1);
	} else if (strcmp(interface, "wl_shm") == 0) {
		d->shm = wl_registry_bind(registry,
					  id, &wl_shm_interface, 1);
		wl_shm_add_listener(d->shm, &_st_shm_listenter, d);
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

struct pilot_display *
pilot_display_create(void)
{
	struct pilot_display *display;

	display = malloc(sizeof(*display));
	display->display = wl_display_connect(NULL);
	assert(display->display);

	display->formats = 0;
	display->registry = wl_display_get_registry(display->display);
	wl_registry_add_listener(display->registry,
				 &_st_registry_listener, display);
	wl_display_roundtrip(display->display);
	if (display->shm == NULL) {
		fprintf(stderr, "No wl_shm global\n");
		exit(1);
	}

	wl_display_roundtrip(display->display);

	if (!(display->formats & (1 << WL_SHM_FORMAT_XRGB8888))) {
		fprintf(stderr, "WL_SHM_FORMAT_XRGB32 not available\n");
		exit(1);
	}

	wl_display_get_fd(display->display);

	memset(display->windows, 0, MAXWINDOWS * sizeof(void *));
	return display;
}

void
pilot_display_destroy(struct pilot_display *display)
{
	if (display->shm)
		wl_shm_destroy(display->shm);

	if (display->shell)
		wl_shell_destroy(display->shell);

	if (display->compositor)
		wl_compositor_destroy(display->compositor);

	wl_registry_destroy(display->registry);
	wl_display_flush(display->display);
	wl_display_disconnect(display->display);
	free(display);
}

int
pilot_display_add_window(struct pilot_display *display, struct pilot_window *window)
{
	int i;

	/**
	 * seach the first window free entry on display
	 **/
	for (i = 0; i < MAXWINDOWS; i++)
		if (display->windows[i] == NULL)
			break;
	if (i < MAXWINDOWS)
	{
		display->windows[i] = window;
		wl_surface_damage(window->surface, 0, 0,
			  window->width, window->height);
	
		_pilot_display_redraw(window, NULL, 0);
	}

	return 0;
}

int
pilot_display_mainloop(struct pilot_display *display)
{
	int ret = 0;

	running = 1;
	while (running && ret != -1)
		ret = wl_display_dispatch(display->display);

	return ret;
}

