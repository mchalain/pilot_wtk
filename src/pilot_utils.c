#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pilot_atk.h>
#include <pilot_utk.h>

static int
_pilot_blit_fill(struct pilot_blit *thiz, pilot_color_t color);
static int
_pilot_blit_copy(struct pilot_blit *thiz, struct pilot_blit *src);

struct pilot_blit *
pilot_blit_create(void *data, pilot_rect_t rect)
{
	PILOT_CREATE_THIZ(pilot_blit);
	thiz->data = data;
	memcpy(&thiz->rect, &rect, sizeof(thiz->rect));
	thiz->action.fill =_pilot_blit_fill;
	thiz->action.copy =_pilot_blit_copy;
	
	return thiz;
}

void
pilot_blit_destroy(struct pilot_blit *thiz)
{
	if (thiz->action.destroy)
		thiz->action.destroy(thiz);
	free(thiz);
}

int
pilot_blit_fill(struct pilot_blit *thiz, pilot_color_t color)
{
	int ret;
	if (thiz->action.fill)
		ret = thiz->action.fill(thiz, color);
	return ret;
}

int
pilot_blit_copy(struct pilot_blit *thiz, struct pilot_blit *src)
{
	int ret;
	if (thiz->action.copy)
		ret = thiz->action.copy(thiz, src);
	return ret;
}

#define MEMSET(s, ll, l) 	{ \
	typeof(ll) *t = (typeof(ll) *)s; \
	while (t < (typeof(ll) *)s + l) { *t++ = ll;} \
	return (t - (typeof(ll) *)s); \
}

inline int memset32(void *s, uint32_t ll, uint32_t l)
MEMSET(s, ll, l)

inline int memset16(void *s, uint16_t ll, uint32_t l)
MEMSET(s, ll, l)

static int
_pilot_blit_fill(struct pilot_blit *thiz, pilot_color_t color)
{
	unsigned int size;
	size = thiz->rect.w * thiz->rect.h;
	memset32(thiz->data, color, size);
	return 0;
}

static int
_pilot_blit_copy(struct pilot_blit *thiz, struct pilot_blit *src)
{
	pilot_color_t *color = (pilot_color_t *)thiz->data;
	void *line = thiz->data + (src->rect.x * sizeof(*color));
	pilot_color_t *srccolor = (pilot_color_t *)src->data;
	pilot_length_t stride = thiz->rect.w * sizeof(*color);
	pilot_length_t len = src->rect.w * sizeof(*color);
	while (((void *)srccolor < (src->data + (len * src->rect.h))) &&
		((void *)color < (thiz->data + (stride * thiz->rect.h))))
	{
		color += src->rect.x;
		while ((void *)color < (line + len)) *color++ = *srccolor++;
		color = line += stride;
	}
	return 0;
}
