#include <stdint.h>
#include <pilot_utk.h>

inline int memset32(void *s, uint32_t ll, uint32_t l)
MEMSET(s, ll, l)

inline int memset16(void *s, uint16_t ll, uint32_t l)
MEMSET(s, ll, l)
