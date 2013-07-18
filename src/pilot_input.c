#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <pilot_wtk.h>

static int
_platform_input_create(struct pilot_input *input, struct pilot_display *display, pilot_key_t id);
static void
_platform_input_destroy(struct pilot_input *display);

struct pilot_input *
pilot_input_create(struct pilot_display *display, pilot_key_t id)
{
	struct pilot_input *input;

	input = malloc(sizeof (*input));
	memset(input, 0, sizeof(*input));

	input->id = id;
	input->display = display;
	_platform_input_create(input, display, id);

	return input;
}

void
pilot_input_destroy(struct pilot_input *input)
{
	_platform_input_destroy(input);
	free(input);
}

#include "platform_input.c"
