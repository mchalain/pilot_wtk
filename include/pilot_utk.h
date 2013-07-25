#ifndef __PILOT_UTK_H__
#define __PILOT_UTK_H__

#define MEMSET(s, ll, l) 	{ \
	typeof(ll) *t = (typeof(ll) *)s; \
	while (t < (typeof(ll) *)s + l) { *t++ = ll;} \
	return (t - (typeof(ll) *)s); \
}

#ifndef memset32
inline int memset32(void *s, uint32_t ll, uint32_t l)
MEMSET(s, ll, l)
#endif
#ifndef memset16
inline int memset16(void *s, uint16_t ll, uint32_t l)
MEMSET(s, ll, l)
#endif

#endif
