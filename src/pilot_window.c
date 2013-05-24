#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pilot_wtk.h>

static struct pilot_buffer *
_pilot_window_next_buffer(struct pilot_window *window);
static int
_pilot_window_add_buffer(struct pilot_window *window, int format);

static void
_handle_ping(void *data, struct wl_shell_surface *shell_surface,
							uint32_t serial)
{
	wl_shell_surface_pong(shell_surface, serial);
}

static void
_handle_configure(void *data, struct wl_shell_surface *shell_surface,
		 uint32_t edges, int32_t width, int32_t height)
{
}

static void
_handle_popup_done(void *data, struct wl_shell_surface *shell_surface)
{
}

static const struct wl_shell_surface_listener _st_shell_surface_listener = {
	_handle_ping,
	_handle_configure,
	_handle_popup_done
};

struct pilot_window *
pilot_window_create(struct pilot_display *display, char *name, int width, int height)
{
	struct pilot_window *window;

	window = calloc(1, sizeof *window);
	if (!window)
		return NULL;

	window->callback = NULL;
	window->display = display;
	window->width = width;
	window->height = height;
	window->surface = wl_compositor_create_surface(display->compositor);
	window->shell_surface = wl_shell_get_shell_surface(display->shell,
							   window->surface);

	if (window->shell_surface)
		wl_shell_surface_add_listener(window->shell_surface,
					      &_st_shell_surface_listener, window);

	if (name)
		wl_shell_surface_set_title(window->shell_surface, name);

	wl_shell_surface_set_toplevel(window->shell_surface);

	return window;
}

void
pilot_window_destroy(struct pilot_window *window)
{
	int i;
	if (window->callback)
		wl_callback_destroy(window->callback);

	for (i = 0; i < MAXBUFFERS; i++)
		if (window->buffers[i])
			pilot_buffer_destroy(window->buffers[i]);

	wl_shell_surface_destroy(window->shell_surface);
	wl_surface_destroy(window->surface);
	free(window);
}

int
pilot_window_set_canvas(struct pilot_window *window, struct pilot_canvas *canvas)
{
	if (!window->canvas)
		window->canvas = canvas;
	_pilot_window_add_buffer(window, canvas->format);
	return -(!(window->canvas == canvas));
}

int
pilot_window_redraw(struct pilot_window *window)
{
	struct pilot_buffer *buffer;
	int x = 20, y=20;

	if (window->canvas)
	{
		
		buffer = _pilot_window_next_buffer(window);
		if (!buffer) {
			fprintf(stderr,
				"Both buffers busy at redraw(). Server bug?\n");
			abort();
		}
		if (window->canvas->draw_handler)
			(*window->canvas->draw_handler)(window->canvas->draw_data, buffer->shm_data);
		wl_surface_attach(window->surface, buffer->buffer, 0, 0);
		wl_surface_damage(window->surface,
			  x, y, window->width - (x*2), window->height - (y*2));
		buffer->busy = 1;
	}
	if (window->eglcanvas)
	{
	}


	return 0;
}


#define SHM_FORMAT_SIZE(format)	_s_shm_format_size[format]
static int _s_shm_format_size[] = { 4, 4};
static int
_pilot_window_add_buffer(struct pilot_window *window, int format)
{
	struct pilot_buffer *buffer;
	int i;

	for (i = 0; i< MAXBUFFERS; i++) if (window->buffers[i] == NULL) break;
	if (i == MAXBUFFERS)
		return -1;
	buffer = pilot_buffer_create(window->display,
				window->width, window->height, format);
	window->buffers[i] = buffer;

	if (!buffer)
		return -1;

	// paint the padding
	memset(buffer->shm_data, 0xff,
		   window->width * window->height * SHM_FORMAT_SIZE(format));
	window->buffers[i] = buffer;
	return 0;
}
static struct pilot_buffer *
_pilot_window_next_buffer(struct pilot_window *window)
{
	struct pilot_buffer *buffer = NULL;
	int i = 0;

	for (i = 0; i < MAXBUFFERS; i++)
		if (!window->buffers[i]->busy) {
			buffer = window->buffers[i];
			break;
		}
	window->next_buffer = buffer;
	return buffer;
}

