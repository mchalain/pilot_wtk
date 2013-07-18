#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <pilot_atk.h>

#define MAX(a,b)	((a) > (b))? (a): (b)

struct pilot_connector *
pilot_connector_create(void *parent)
{
	struct pilot_connector *connector;
	connector = malloc(sizeof(*connector));
	memset(connector, 0, sizeof(*connector));
	connector->parent = parent;
	return connector;
}

void
pilot_connector_destroy(struct pilot_connector *connector)
{
	free(connector);
}

struct pilot_application *
pilot_application_create(int argc, char **argv)
{
	struct pilot_application *application;
	application = malloc(sizeof(*application));
	memset(application, 0, sizeof(*application));
	return application;
}

void
pilot_application_destroy(struct pilot_application *application)
{
	pilot_list_destroy(application->connectors);
	free(application);
}

int
pilot_application_addconnector(struct pilot_application *application,
						struct pilot_connector *connector)
{
	pilot_list_append(application->connectors, connector);
	return 0;
}

static int
_pilot_application_check_fdset(struct pilot_application *application,
							struct pilot_connector *connector)
{
	if (FD_ISSET(connector->fd, &application->rfds)) {
		if (connector->action.dispatch_events)
			connector->action.dispatch_events(connector->parent);
	}
	return 0;
}

int
pilot_application_dispatchevents(struct pilot_application *application)
{
	pilot_list_foreach(application->connectors, 
			_pilot_application_check_fdset, application);
	return 0;
}

static int
_pilot_application_fill_fdset(struct pilot_application *application,
							struct pilot_connector *connector)
{
	FD_SET(connector->fd, &application->rfds);
	application->maxfd = MAX(application->maxfd, connector->fd + 1);
	if (connector->action.prepare_wait)
		connector->action.prepare_wait(connector->parent);
	return 0;
}

int
pilot_application_run(struct pilot_application *application)
{
	int ret = 0;

	application->running = 1;
	while (application->running && ret != -1) {
		FD_ZERO(&application->rfds);
		pilot_list_foreach(application->connectors, 
				_pilot_application_fill_fdset, application);

		ret = select(application->maxfd, &application->rfds, NULL, NULL, NULL);

		if (ret == -1 && errno == EINTR) {
			ret = 0;
		}
	}

	return ret;
}

int
pilot_application_exit(struct pilot_application *application, int ret)
{
	application->running = 0;
	return ret;
}
