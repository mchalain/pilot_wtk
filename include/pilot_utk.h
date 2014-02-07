#ifndef __PILOT_UTK_H__
#define __PILOT_UTK_H__

struct pilot_blit
{
	void *data;
	pilot_rect_t rect;
	struct
	{
		void (*destroy)(struct pilot_blit *thiz);
		int (*fill)(struct pilot_blit *thiz, pilot_color_t color);
		int (*copy)(struct pilot_blit *thiz, struct pilot_blit * src);
	} action;
};

struct pilot_blit *
pilot_blit_create(void *data, pilot_rect_t rect);
void
pilot_blit_destroy(struct pilot_blit *thiz);
int
pilot_blit_fill(struct pilot_blit *thiz, pilot_color_t color);
int
pilot_blit_copy(struct pilot_blit *thiz, struct pilot_blit *src);


#endif
