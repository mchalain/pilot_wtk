static void
pointer_handle_enter(void *data, struct wl_pointer *pointer,
		     uint32_t serial, struct wl_surface *surface,
		     wl_fixed_t sx, wl_fixed_t sy)
{
	struct pilot_input *input = (struct pilot_input *)data;
	struct pilot_window *window = _platform_display_search_window(input->display, surface);
	window->common.hasmouse = 1;
	pilot_emit(input, mouseEntered, window, 1);
}

static void
pointer_handle_leave(void *data, struct wl_pointer *pointer,
		     uint32_t serial, struct wl_surface *surface)
{
	struct pilot_input *input = (struct pilot_input *)data;
	struct pilot_window *window = _platform_display_search_window(input->display, surface);
	window->common.hasmouse = 0;
	window->common.hasclick = 0;
	pilot_emit(input, mouseEntered, window, 0);
}

static void
pointer_handle_motion(void *data, struct wl_pointer *pointer,
		      uint32_t time, wl_fixed_t sx, wl_fixed_t sy)
{
	struct pilot_input *input = (struct pilot_input *)data;
	pilot_emit(input, mouse_scrolled, (int32_t)sx, (int32_t)sy);
}

static void
pointer_handle_button(void *data, struct wl_pointer *wl_pointer,
		      uint32_t serial, uint32_t time, uint32_t button,
		      uint32_t state)
{
	struct pilot_input *input = (struct pilot_input *)data;
	pilot_emit(input, mouse_clicked, button, state);

	typeof (input->display->windows) *windows_it = &input->display->windows;
	while (windows_it->item) {
		struct pilot_widget *widget = (struct pilot_widget *)windows_it->item;
		if (widget->hasmouse) {
			if (state)
				widget->hasclick = 1;
			else if (widget->hasclick)
				pilot_emit(widget, clicked, button);
			break;
		}
		windows_it = windows_it->next;
	}
}

static void
pointer_handle_axis(void *data, struct wl_pointer *wl_pointer,
		    uint32_t time, uint32_t axis, wl_fixed_t value)
{
	struct pilot_input * input = (struct pilot_input *)data;
	pilot_emit(input, mouse_moved, axis, value);
}

static const struct wl_pointer_listener _st_pointer_listener = {
	pointer_handle_enter,
	pointer_handle_leave,
	pointer_handle_motion,
	pointer_handle_button,
	pointer_handle_axis,
};

