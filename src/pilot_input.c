#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <pilot_atk.h>
#include <pilot_utk.h>
#include <pilot_wtk.h>

#include "pilot_wtk_internal.h"

static void *
_platform_input_create(struct pilot_display *display);
static void *
_platform_inputkeyboard_create(struct pilot_input *input, struct pilot_display *display);
static void *
_platform_inputpointer_create(struct pilot_input *input, struct pilot_display *display);
static void
_platform_input_destroy(struct pilot_input *display);

struct pilot_input_event
{
	enum {event_key, event_mouse, event_mousewheel} type;
	union
	{
		struct
		{
			short m_keycode;
			long m_value;
			char m_action;
		} key;
		struct
		{
			long m_xdelta;
			long m_ydelta;
		} mouse;
		struct 
		{
			long m_xscroll;
			long m_yscroll;
		} mousewheel;
	} data;
};

struct pilot_input *
pilot_inputpointer_create(struct pilot_display *display)
{
	struct pilot_input *thiz = pilot_input_create(display);
#ifdef HAVE_POINTER
	thiz->platform = (void *)_platform_inputpointer_create(thiz, display);
#endif
	return thiz;
}

struct pilot_input *
pilot_inputkeyboard_create(struct pilot_display *display)
{
	struct pilot_input *thiz = pilot_input_create(display);
#ifdef HAVE_KEYBOARD
	thiz->platform = (void *)_platform_inputkeyboard_create(thiz, display);
#endif
	return thiz;
}

struct pilot_input *
pilot_input_create(struct pilot_display *display)
{
	PILOT_CREATE_THIZ(pilot_input);

	thiz->display = display;

	return thiz;
}

void
pilot_input_destroy(struct pilot_input *thiz)
{
	_platform_input_destroy(thiz);
	free(thiz);
}

int
pilot_input_focus(struct pilot_input *thiz, struct pilot_window *window)
{
	thiz->window = window;
	return 0;
}
#include "platform_input.c"
