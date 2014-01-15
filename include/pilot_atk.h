#ifndef __PILOT_ATK_H__
#define __PILOT_ATK_H__

#include <stdint.h>
#include "pilot_types.h"
#include "pilot_log.h"
#include "pilot_signal.h"

typedef void pilot_object_t;


#ifndef _pilot_list
#define _pilot_list(type, name) struct _pilot_list_##type {\
			struct type *item; \
			struct _pilot_list_##type *next; \
		} name
#define pilot_list_append(list, entry) do { typeof (list) *it = &list; \
			while (it->next) it = it->next; \
			it->item = entry; \
			it->next = malloc(sizeof(typeof (list))); \
			it = it->next; \
			memset(it, 0, sizeof(typeof (list))); \
		} while(0)
#define pilot_list_foreach(list, func, data) do { typeof (list) *it = &list; \
			while (it->next) { \
				func(data, it->item); \
				it = it->next; \
			} \
		} while(0)
#define pilot_list_destroy(list) do { typeof (list) *it = &list; \
			it = it->next; \
			while (it->next) { \
				typeof (list) *tmp = it->next; \
				free(it); \
				it = tmp; \
			} \
			free(it); \
		} while(0)
#endif

struct pilot_connector
{
	pilot_object_t *parent;
	int fd;
	pilot_bool_t distribut;
	struct {
		int (*prepare_wait)(void *);
		int (*dispatch_events)(void *);
	} action;
};

struct pilot_connector *
pilot_connector_create(void *parent);
void
pilot_connector_destroy(struct pilot_connector *connector);

struct pilot_application
{
	_pilot_list(pilot_connector, connectors);
	fd_set rfds;
	int maxfd;
	int signal_pipe[2];
	pilot_bool_t running:1;
	pilot_bool_t dispatch:1;
};

struct pilot_application *
pilot_application_create(int argc, char **argv);
void
pilot_application_destroy(struct pilot_application *application);
int
pilot_application_addconnector(struct pilot_application *application,
						struct pilot_connector *connector);
int
pilot_application_check(struct pilot_application *application);
int
pilot_application_dispatchevents(struct pilot_application *application);
int
pilot_application_run(struct pilot_application *application);
int
pilot_application_exit(struct pilot_application *application, int ret);

struct pilot_timer
{
	struct pilot_connector *connector;
	_pilot_signal(pilot_timer, ring, int id);
};

struct pilot_timer *
pilot_timer_create(struct pilot_application *application);
void
pilot_timer_destroy(struct pilot_timer *timer);

#endif
