struct pilot_cursor *
pilot_cursor_create(struct pilot_display* display);
{
	struct pilot_cursor *cursor;

	cursor = malloc(sizeof (*cursor));
	memset(cursor, 0, sizeof (*cursor));

	cursor->platform.cursor_theme = wl_cursor_theme_load(NULL, 32, display->platform.shm);
	cursor->platform.default_cursor =
		wl_cursor_theme_get_cursor(cursor->platform.theme, "left_ptr");

	cursor->platform.surface =
		wl_compositor_create_surface(display->platform.compositor);

	pilot_widget_connect(display, SIGNAL(mouse_enter), cursor, SLOT(_pilot_cursor_enter));
	return cursor;
}

void
pilot_cursor_destroy(struct pilot_cursor *cursor)
{
	wl_surface_destroy((cursor->platform.surface);
	if ((cursor->platform.theme)
		wl_cursor_theme_destroy((cursor->platform.theme);
}

static void
_pilot_cursor_enter(struct pilot_cursor *cursor)
{
	struct wl_buffer *buffer;
	struct wl_cursor *plat_cursor = cursor->platform.cursor;
	struct wl_cursor_image *image;

	if (display->window->fullscreen)
		wl_pointer_set_cursor(pointer, serial, NULL, 0, 0);
	else if (plat_cursor) {
		image = plat_cursor->images[0];
		buffer = wl_cursor_image_get_buffer(image);
		wl_pointer_set_cursor(pointer, serial,
					  cursor->platform.cursor_surface,
					  image->hotspot_x,
					  image->hotspot_y);
		wl_surface_attach(pcursor->platform.cursor_surface, buffer, 0, 0);
		wl_surface_damage(pcursor->platform.cursor_surface, 0, 0,
				  image->width, image->height);
		wl_surface_commit(pcursor->platform.cursor_surface);
	}
}
