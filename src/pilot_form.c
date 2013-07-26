#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pilot_wtk.h>
#include <linux/input.h>
#include <dlfcn.h>

static void
_pilot_form_destroy(void *widget);
static int
_pilot_form_show(void *widget);
static int
_pilot_form_redraw(void *widget);
static int
_pilot_form_resize(void *widget, uint32_t width, uint32_t height);
static int
_pilot_form_keyEvent(struct pilot_widget *widget, pilot_key_t key);
static int
_pilot_form_mouseEvent(struct pilot_widget *widget, pilot_key_t key);
static pilot_color_t *
_pilot_form_search_resources(struct pilot_form *form, int type, int id);

struct pilot_form *
pilot_form_create(struct pilot_widget *parent, int id)
{
	struct pilot_form *form;

	form = malloc(sizeof *form);
	if (!form)
		return NULL;
	memset(form, 0, sizeof(*form));
	pilot_widget_init(&form->common, parent);
	pilot_rect_copy(&form->common.region, &parent->region);
	form->common.action.show = _pilot_form_show;
	form->common.action.redraw = _pilot_form_redraw;
	form->common.action.resize = _pilot_form_resize;
	form->common.action.destroy = _pilot_form_destroy;

	form->image = _pilot_form_search_resources(form, PILOT_RESOURCE_FORM,id);
	form->surface = parent->window->surfaces[0];

	struct pilot_widget *widget = (struct pilot_widget *)form;
	pilot_widget_grabkeys(widget, 1);
	pilot_connect(widget, keyPressed, widget, _pilot_form_keyEvent);
	struct pilot_widget *window = (struct pilot_widget *)form->common.window;
	pilot_connect(window, clicked, widget, _pilot_form_mouseEvent);

	if (!form->surface)
	{
		free(form);
		return NULL;
	}
	return form;
}

static void
_pilot_form_destroy(void *widget)
{
	int i;
	struct pilot_form *form = widget;

	free(form);
}

void
pilot_form_destroy(struct pilot_form *form)
{
	_pilot_form_destroy(form);
}

static int
_pilot_form_keyEvent(struct pilot_widget *widget, pilot_key_t key)
{
	if (key == KEY_ESC) {
		//pilot_application_exit(g_application, 0);
	} else {
		printf("press %d\n", key);
	}
	return 0;
}

static int
_pilot_form_mouseEvent(struct pilot_widget *widget, pilot_key_t key)
{
	printf("click %d\n", key);
	
	return 0;
}

static int
_pilot_form_redraw(void *widget)
{
	int ret = 0;
	struct pilot_form *form = widget;
	if (form->redraw) {
		struct pilot_window *window = form->common.window;
		ret = 1;
		form->redraw = 0;
		pilot_surface_paint_window(form->surface, window);
	}
	return ret;
}

static int
_pilot_form_resize(void *widget, uint32_t width, uint32_t height)
{
	struct pilot_form *form = widget;
	form->redraw = 1;
	return 0;
}

int test = 1;
static int
_pilot_form_show(void *widget)
{
	struct pilot_form *form = widget;
	int y;
	uint32_t *pixel = form->surface->data;

	for (y = 0; y < form->common.region.h; y++) {
		int x;
		for (x = 0; x < form->common.region.w; x++) {
			uint32_t v;
			v=*(form->image)++;
			if (test) {
				LOG_DEBUG("%p 0x%08X 0x%08X",form->image, *form->image, v);
				test = 0;
			}
			*pixel++ = v;
		}
	}
	form->redraw = 1;
	return 0;
}

static pilot_color_t *
_pilot_form_search_resources(struct pilot_form *form, int type, int id)
{
	int i;
	pilot_color_t *image;
	void    *handle;
	struct pilot_resource *resources;
	int nb_resources;
	int (*resources_init)(void);

	handle = dlopen(NULL, RTLD_LOCAL | RTLD_NOW);
	resources = (struct pilot_resource *)dlsym(handle, "PILOT_resources");
	nb_resources = (int)dlsym(handle, "PILOT_resources_nb");
	resources_init = (int)dlsym(handle, "PILOT_resources_init");
	if (resources_init == NULL) {
		LOG_ERROR("not resource available: %s", dlerror());
		return NULL;
	}
	(*resources_init)();
	dlclose(handle);

	for (i = 0; i < nb_resources; i++) {
		if (resources[i].type == type &&
				resources[i].id == id) {
			pilot_length_t h, w;
			image = resources[i].image;
			w = resources[i].region.w;
			h = resources[i].region.h;

			pilot_widget_resize((struct pilot_widget *)form, w, h);
			break;
		}
	}

	return image;
}
