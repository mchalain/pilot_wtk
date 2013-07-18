#include <linux/input.h>

static void
keyboard_handle_keymap(void *data, struct wl_keyboard *keyboard,
		       uint32_t format, int fd, uint32_t size)
{
	struct pilot_input *input = (struct pilot_input *)data;
}

static void
keyboard_handle_enter(void *data, struct wl_keyboard *keyboard,
		      uint32_t serial, struct wl_surface *surface,
		      struct wl_array *keys)
{
	struct pilot_input *input = (struct pilot_input *)data;
	struct pilot_window *window;

	window = _platform_display_search_window(input->display, surface);
	if (window) {
		window->common.hasfocus = 1;
		pilot_emit(input->display,focusChanged, window, 1);
	}
}

static void
keyboard_handle_leave(void *data, struct wl_keyboard *keyboard,
		      uint32_t serial, struct wl_surface *surface)
{
	struct pilot_input *input = (struct pilot_input *)data;
	struct pilot_window *window;

	window = _platform_display_search_window(input->display, surface);
	if (window) {
		window->common.hasfocus = 0;
		pilot_emit(input->display,focusChanged, window, 0);
	}
}

static void
keyboard_handle_key(void *data, struct wl_keyboard *keyboard,
		    uint32_t serial, uint32_t time, uint32_t key,
		    uint32_t state)
{
	struct pilot_input *input = (struct pilot_input *)data;

	pilot_emit(input,keyChanged, key, state);
}

static void
keyboard_handle_modifiers(void *data, struct wl_keyboard *keyboard,
			  uint32_t serial, uint32_t mods_depressed,
			  uint32_t mods_latched, uint32_t mods_locked,
			  uint32_t group)
{
	struct pilot_input *input = (struct pilot_input *)data;
}

static const struct wl_keyboard_listener _st_keyboard_listener = {
	keyboard_handle_keymap,
	keyboard_handle_enter,
	keyboard_handle_leave,
	keyboard_handle_key,
	keyboard_handle_modifiers,
};

