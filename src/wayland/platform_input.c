#include "platform_wtk.h"

#ifdef HAVE_POINTER
#include "platform_display_pointer.c"
#endif
#ifdef HAVE_KEYBOARD
#include "platform_display_keyboard.c"
#endif

static int
_platform_input_create(struct pilot_input *input, struct pilot_display *display, pilot_key_t id)
{
	struct platform_input *platform = malloc(sizeof(*platform));
	struct platform_display *pl_display = display->platform;

	platform->seat = pl_display->seat;
	if (id == PILOT_INPUT_POINTER) {
		platform->pointer = wl_seat_get_pointer(platform->seat);
		wl_pointer_add_listener(platform->pointer, &_st_pointer_listener, input);
	}
	if (id == PILOT_INPUT_KEYBOARD) {
		platform->keyboard = wl_seat_get_keyboard(platform->seat);
		wl_keyboard_add_listener(platform->keyboard, &_st_keyboard_listener, input);
	}
	input->platform = platform;
	return 0;
}

static void
_platform_input_destroy(struct pilot_input *input)
{
	struct platform_input *platform = input->platform;
	if (platform->pointer) {
		wl_pointer_destroy(platform->pointer);
		platform->pointer = NULL;
	}
	free(platform);
}

