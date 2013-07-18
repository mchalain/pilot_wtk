#include <stdint.h>

inline int memset32(void *s, uint32_t ll, uint32_t l)
{
	uint32_t *t = (uint32_t *)s;
	while ((void *)t < s + l) { *t = ll; t++;}
	return ((void *)t - s);
}

inline int memset16(void *s, uint16_t ll, uint32_t l)
{
	uint16_t *t = (uint16_t *)s;
	while ((void *)t < s + l) { *t = ll; t++;}
	return ((void *)t - s);
}
