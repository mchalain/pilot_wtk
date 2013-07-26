#ifndef __PILOT_SIGNAL_H__
#define __PILOT_SIGNAL_H__

#define _pilot_signal(obj, signal,...) \
	struct { \
		struct obj##_##signal##_slot{ \
			int(*action)(struct pilot_widget *, ##__VA_ARGS__); \
			struct pilot_widget *dest; \
			struct obj##_##signal##_slot *next; \
		} slots; \
	} signal

#define pilot_connect(src, signal, dst, slot) \
	do { \
		typeof(src->signal.slots) *slot_it = &src->signal.slots; \
		while (slot_it->next) { \
			if ((void *)slot_it->next->dest == (void *)dst && (void *)slot_it->next->action == (void *)slot) \
				break; \
			slot_it = slot_it->next; \
		} \
		if (!slot_it->next) { \
			slot_it->next = malloc(sizeof(src->signal.slots)); \
			memset(slot_it->next, 0, sizeof(src->signal.slots)); \
			slot_it->next->dest = (struct pilot_widget *)dst; \
			slot_it->next->action = slot; \
		} \
	} while(0)

#define pilot_disconnect(src, signal, dst, slot) \
	do { \
		typeof(src->signal.slots) *slot_it = &src->signal.slots; \
		while (slot_it->next) { \
			if ((void *)slot_it->next->dest == (void *)dst && (void *)slot_it->next->action == (void *)slot) { \
				typeof(src->signal.slots) *tmp = slot_it->next; \
				slot_it->next = tmp->next; \
				free(tmp); \
			} \
			else \
				slot_it = slot_it->next; \
		} \
	} while(0)

#define pilot_emit(src, signal, ...) \
	do { \
		typeof(src->signal.slots) *slot_it = &src->signal.slots; \
		while (slot_it->next) { \
			slot_it = slot_it->next; \
			slot_it->action(slot_it->dest, ##__VA_ARGS__); \
		} \
	} while(0)

#define pilot_emit_to(src, signal, dst, ...) \
	do { \
		typeof(src->signal.slots) *slot_it = &src->signal.slots; \
		while (slot_it->next) { \
			slot_it = slot_it->next; \
			if ((void *)dst == (void *)slot_it->dest) \
				slot_it->action(slot_it->dest, ##__VA_ARGS__); \
		} \
	} while(0)

#endif
