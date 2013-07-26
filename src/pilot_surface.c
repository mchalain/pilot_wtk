#include <stdlib.h>
#include <pilot_wtk.h>

#define SHM_FORMAT_SIZE(format)	_s_shm_format_size[format]
static int _s_shm_format_size[] = { sizeof(pilot_color_t), sizeof(pilot_color_t)};

int
pilot_surface_init(struct pilot_surface *surface,
					pilot_rect_t region,
					pilot_pixel_format_t format)
{
	int pixel;

	surface->width = region.w;
	surface->height = region.h;
	surface->format = format;
	pixel = SHM_FORMAT_SIZE(surface->format);
	surface->stride = surface->width * pixel;
	surface->size =  surface->height * surface->stride;
	mutex_init(surface->lock, NULL);
	cond_init(surface->cond, NULL);
	return 0;
}

void
pilot_surface_destroy(struct pilot_surface *surface)
{
	if (surface->action.destroy)
		surface->action.destroy(surface);
	mutex_destroy(surface->lock);
	cond_destroy(surface->cond);
	surface->busy =0;
	free(surface);
}

int
pilot_surface_lock(struct pilot_surface *surface, void **image)
{
	int ret;
	if (!(ret = mutex_lock(surface->lock))) {
		while ( surface->busy || surface->ready) {
			LOG_DEBUG("ret %d",ret);
			ret = cond_wait(surface->cond, surface->lock);
			if (ret)
				break;
		}
		*image = (void *)surface->data;
	}
	return ret;
}

void 
pilot_surface_unlock(struct pilot_surface *surface)
{
	surface->ready = 1;
	mutex_unlock(surface->lock);
}

void
pilot_surface_release(struct pilot_surface *surface)
{
	if (!mutex_lock(surface->lock)) {
		surface->busy=0;
		cond_signal(surface->cond);
		mutex_unlock(surface->lock);
	}
}

void
pilot_surface_paint_window(struct pilot_surface *surface, struct pilot_window *window)
{
	LOG_DEBUG("");
	if (! mutex_lock(surface->lock)) {
		if (surface->action.paint_window)
			surface->action.paint_window(surface, window);
		LOG_DEBUG("");
		surface->busy=1;
		surface->ready = 0;
		mutex_unlock(surface->lock);
	}
}

#ifndef HAVE_INLINE
uint32_t
pilot_surface_size(struct pilot_surface *surface)
{
	return surface->size;
}

pilot_bool_t
pilot_surface_busy(struct pilot_surface *surface)
{
	return surface->busy;
}

pilot_bool_t
pilot_surface_ready(struct pilot_surface *surface)
{
	return surface->ready;
}

#endif
