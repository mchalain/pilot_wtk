pilot_wayland
=============

simple toolkit to create wayland clients

This toolkit is written in C but use objects architecture.
A system of signal and slot is available to connect event to a function.

1) Object reference
1.1) pilot_widget
 this object is defined for all other object too.
 signals:
	- clicked(pilot_key_t button)
		emit when the user click inside the widget.
	- scrolled( pilot_coord_t x, pilot_coord_t y);
		emit when the mouse scroll rolled inside the widget.
	- keyPressed( pilot_key_t key);
		emit when key pressed inside the widget.
	- focusChanged( pilot_bool_t in);
		emit when the widget grabs or releases the focus.
 functions:
	- pilot_widget *create(pilot_widget *parent)
	- void destroy()
	- int init(struct pilot_widget *parent)
	- int redraw()
	- int size(pilot_length_t *width, pilot_length_t *height)
	- struct pilot_display *display()
	- void focus()
	- int grabkeys(pilot_bool_t yes)
	- pilot_bool_t hasfocus()
	- pilot_bool_t hasmouse()

1.2) pilot_display
 this object is the main object for application.
 It contains the main loop and requests the access to the windows.
 signals:
	- keyChanged(int key, int state)
		emit when the user press or releases a key.
	- focusChanged(struct pilot_window *window, int in)
		emit when a window which grabs the focus or releases.
	- mouseEntered(struct pilot_window *window, int in)
		emit when a window which grabs the pointer or releases.
 functions:
	- pilot_display *create()
	- void destroy()
	- int add_window(pilot_window *window)
	- int mainloop()
	- int exit(int ret)
	- search_window(f_search_handler search)

1.3) pilot_window
 this object is a window inside the display or inside another window.
 It contains a theme and a main layout. The layout could be a pilot_layout
 or one kind of pilot_widget.
 signals:
	none
 functions:
	- pilot_window *create(pilot_widget *parent, char *name, pilot_length_t width, pilot_length_t height)
	- void destroy()
	- int set_layout(pilot_widget *layout)
	- int resize(pilot_length_t width, pilot_length_t height)
	- struct pilot_widget *layout()
	- int show()
	- int focus()

1.4) pilot_layout
 this object is a widget that contains others widgets
 and places them inside the window.
 signals:
	none
 functions:
	- pilot_layout *create(pilot_widget *parent)
	- void destroy()
	- add_widget(pilot_widget *child)

1.5) pilot_theme
 this object describes the window.
 It is a abstract object.
 signals:
	none
 functions:
	- pilot_theme *create(pilot_window *window)
	- void destroy()
	- pilot_widget *get_caption()
	- pilot_length get_border()
	- int resize_window(pilot_length_t *width, pilot_length_t *height)

1.6) pilot_canvas
 this object is a simple widget to draw pixel by pixel.
 signals:
	none
 functions:
	- pilot_canvas *create(struct pilot_widget *parent, int format)
	- void destroy()
	- int set_draw_handler(f_draw_handler handler, void *data)
	- void *draw_data()

2) signals / slots
 A pilot_wtk can contain signal, emited during the application life (mainloop).
 It is possible to connect one to slot (function callback).
 The signal is emited by a source and the source has to be a object
 which contains the signal.
 The slot is a callback sended to destination instance. This callback
 has the destination object pointer as argument and the signal arguments.
 functions (macros):
	- pilot_connect(src, signal, dest, slot)
	- pilot_disconnect(src, signal, dest, slot)
	- pilot_emit(src, signal,...)
	- pilot_emit_to(src, signal, dst, ...)
 objects:
	- pilot_signal(name, arguments...)
	- slot(pilot_widget *dest, signal_arguments)

3) Examples:
int main(int argc, char **argv)
{
	struct pilot_display *display;
	struct pilot_window *mainwindow;
	struct pilot_window *mainwindow2;

	display = pilot_display_create();
	mainwindow = pilot_window_create((struct pilot_widget *)display, argv[0], 250, 250);
	pilot_display_add_window(display, mainwindow);

	pilot_window_show(mainwindow);
	pilot_display_mainloop(display);

	pilot_window_destroy(mainwindow);
	pilot_display_destroy(display);
	return 0;
}
