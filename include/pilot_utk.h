#ifndef __PILOT_UTK_H__
#define __PILOT_UTK_H__

struct pilot_memory
{
	void *data;
	pilot_rect_t rect;
	struct
	{
		void (*destroy)(struct pilot_memory *thiz);
		int (*fill)(struct pilot_memory *thiz, pilot_color_t color);
		int (*copy)(struct pilot_memory *thiz, struct pilot_memory * src);
	} action;
};

struct pilot_memory *
pilot_memory_create(void *data, pilot_rect_t rect);
void
pilot_memory_destroy(struct pilot_memory *thiz);
int
pilot_memory_fill(struct pilot_memory *thiz, pilot_color_t color);
int
pilot_memory_copy(struct pilot_memory *thiz, struct pilot_memory *src);


#endif
