#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/timerfd.h>
#include <pilot_atk.h>

static int
_pilot_timer_dispatch_events(struct pilot_timer *timer);

struct pilot_timer *
pilot_timer_create(struct pilot_application *application)
{
	struct pilot_timer *timer;

	timer = calloc(1, sizeof *timer);
	if (!timer)
		return NULL;
	memset(timer, 0, sizeof(*timer));

	timer->connector = 
		pilot_connector_create(timer);
	timer->connector->fd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC);
	//timer->connector->action.prepare_wait = _pilot_timer_prepare_wait;
	timer->connector->action.dispatch_events = _pilot_timer_dispatch_events;
	pilot_application_addconnector(application, timer->connector);

	return timer;
}

void
pilot_timer_destroy(struct pilot_timer *timer)
{
	if (timer->connector->fd)
		close(timer->connector->fd);
	pilot_connector_destroy(timer->connector);
	free(timer);
}

static int
_pilot_timer_dispatch_events(struct pilot_timer *timer)
{
	int ret = 0;
	int i;
	uint64_t numexp;

	ret = read(timer->connector->fd, &numexp, sizeof(numexp));
	for (i = 0; i < numexp; i++)
		pilot_emit(timer, ring, 0);
	return ret;
}
