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
	struct pilot_window *window = _platform_display_search_window(input->display, surface);
	input->window = window;
	pilot_window_setfocus(window, NULL);
}

static void
keyboard_handle_leave(void *data, struct wl_keyboard *keyboard,
		      uint32_t serial, struct wl_surface *surface)
{
	struct pilot_input *input = (struct pilot_input *)data;
	struct pilot_window *window = _platform_display_search_window(input->display, surface);
	input->window->focus = NULL;
	if (input->window == window)
		input->window = NULL;
}

static void
keyboard_handle_key(void *data, struct wl_keyboard *keyboard,
		    uint32_t serial, uint32_t time, uint32_t key,
		    uint32_t state)
{
	struct pilot_input *input = (struct pilot_input *)data;
	LOG_DEBUG("key 0x%X %p", key, input);
	fprintf(stdout, "%p key 0x%X %p\n", input->window->focus, key, input);
	if (input->window && input->window->focus)
		pilot_emit(input->window->focus, keyChanged, key, state);
}

static void
keyboard_handle_modifiers(void *data, struct wl_keyboard *keyboard,
			  uint32_t serial, uint32_t mods_depressed,
			  uint32_t mods_latched, uint32_t mods_locked,
			  uint32_t group)
{
	struct pilot_input *input = (struct pilot_input *)data;
	LOG_DEBUG("%p mods 0x%X 0x%X 0x%X 0x%X", input, mods_depressed, mods_latched, mods_locked, group);
	if (input->window && input->window->focus)
		pilot_emit(input->window->focus, keyChanged, mods_depressed, 1);
}

static const struct wl_keyboard_listener _st_keyboard_listener = {
	keyboard_handle_keymap,
	keyboard_handle_enter,
	keyboard_handle_leave,
	keyboard_handle_key,
	keyboard_handle_modifiers,
};

static void *
_platform_inputkeyboard_create(struct pilot_input *input, struct pilot_display *display)
{
	PILOT_CREATE_THIZ(platform_input);
	struct platform_display *pl_display = display->platform;
	thiz->keyboard = wl_seat_get_keyboard(pl_display->seat);
	wl_keyboard_add_listener(thiz->keyboard, &_st_keyboard_listener, input);
	return thiz;
}
