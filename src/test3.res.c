#include <pilot_wtk.h>
#include <pilot_utk.h>
#include "test3.res.h"

pilot_color_t g_form[PILOT_DISPLAY_HEIGHT][PILOT_DISPLAY_WIDTH];
int PILOT_resources_nb = PILOT_RESOURCES_MAX;
struct pilot_resource PILOT_resources[PILOT_RESOURCES_MAX] = 
{
	{
		.type = PILOT_RESOURCE_FORM,
		.id = MAINFORM,
		.region = {0, 0, PILOT_DISPLAY_WIDTH, PILOT_DISPLAY_HEIGHT },
		.image = (pilot_color_t *)g_form,
	},
};

int
PILOT_resources_init(void)
{
	int ret;
	ret = colorset(g_form, 0xFF555555, PILOT_DISPLAY_WIDTH * PILOT_DISPLAY_HEIGHT);
}
