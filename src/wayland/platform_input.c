#include "platform_wtk.h"

#ifdef HAVE_POINTER
#include "platform_input_pointer.c"
#endif
#ifdef HAVE_KEYBOARD
#include "platform_input_keyboard.c"
#endif

static void
_seat_handle_capabilities(void *data, struct wl_seat *seat,
			 enum wl_seat_capability caps)
{
	struct pilot_display *display = data;
	struct platform_display *platform = display->platform;
	struct pilot_input *input = NULL;

#ifdef HAVE_POINTER
	if ((caps & WL_SEAT_CAPABILITY_POINTER)) {
		input = pilot_inputpointer_create(display);
	}
#endif
#ifdef HAVE_KEYBOARD
	if ((caps & WL_SEAT_CAPABILITY_KEYBOARD))
	{
		input = pilot_inputkeyboard_create(display);
	}
#endif
	if (input)
		pilot_list_append(display->inputs, input);
	pilot_emit(display, inputChanged, input);
}

static const struct wl_seat_listener _st_seat_listener = {
	_seat_handle_capabilities,
};

void
_platform_display_inputmanager(struct pilot_display *display, struct wl_registry *registry, uint32_t id)
{
	struct platform_display *platform = display->platform;
	platform->seat = wl_registry_bind(registry,
					id, &wl_seat_interface, 1);
	wl_seat_add_listener(platform->seat, &_st_seat_listener, display);
}

static void
_platform_input_destroy(struct pilot_input *thiz)
{
	struct platform_input *platform = thiz->platform;
	if (platform->pointer) {
		wl_pointer_destroy(platform->pointer);
		platform->pointer = NULL;
	}
	if (platform->keyboard) {
		wl_keyboard_destroy(platform->keyboard);
		platform->keyboard = NULL;
	}
	if (platform->touch) {
		wl_touch_destroy(platform->touch);
		platform->touch = NULL;
	}
	free(platform);
}

