static void
pointer_handle_enter(void *data, struct wl_pointer *pointer,
		     uint32_t serial, struct wl_surface *surface,
		     wl_fixed_t sx, wl_fixed_t sy)
{
	struct pilot_input *input = (struct pilot_input *)data;
	struct pilot_window *window = _platform_display_search_window(input->display, surface);
	struct platform_input *platform = input->platform;
	platform->x = sx;
	platform->y = sy;
	input->window = window;
}

static void
pointer_handle_leave(void *data, struct wl_pointer *pointer,
		     uint32_t serial, struct wl_surface *surface)
{
	struct pilot_input *input = (struct pilot_input *)data;
	struct pilot_window *window = _platform_display_search_window(input->display, surface);
	if (input->window == window)
		input->window = NULL;
}

static void
pointer_handle_motion(void *data, struct wl_pointer *pointer,
		      uint32_t time, wl_fixed_t sx, wl_fixed_t sy)
{
	struct pilot_input *input = (struct pilot_input *)data;
	struct platform_input *platform = input->platform;
	platform->x = sx;
	platform->y = sy;
	pilot_emit(input->window->layout, mouseMoved, (int32_t)sx, (int32_t)sy);
}

static void
pointer_handle_button(void *data, struct wl_pointer *wl_pointer,
		      uint32_t serial, uint32_t time, uint32_t button,
		      uint32_t state)
{
	struct pilot_input *input = (struct pilot_input *)data;
	struct platform_input *platform = input->platform;
	struct pilot_widget *child;
	child = pilot_widget_getchildat(input->window->layout, platform->x, platform->y);
	pilot_window_setfocus(input->window, child);
	pilot_emit(child, mousebuttonChanged, button, state);
}

static void
pointer_handle_axis(void *data, struct wl_pointer *wl_pointer,
		    uint32_t time, uint32_t axis, wl_fixed_t value)
{
	struct pilot_input * input = (struct pilot_input *)data;
	pilot_emit(input->window->layout, mousewheelMoved, axis, value);
}

static const struct wl_pointer_listener _st_pointer_listener = {
	pointer_handle_enter,
	pointer_handle_leave,
	pointer_handle_motion,
	pointer_handle_button,
	pointer_handle_axis,
};

static void *
_platform_inputpointer_create(struct pilot_input *input, struct pilot_display *display)
{
	PILOT_CREATE_THIZ(platform_input);
	struct platform_display *pl_display = display->platform;
	thiz->pointer = wl_seat_get_pointer(pl_display->seat);
	wl_pointer_add_listener(thiz->pointer, &_st_pointer_listener, input);
	return thiz;
}
