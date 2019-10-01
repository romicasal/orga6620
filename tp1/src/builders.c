#include <string.h>
#include "builders.h"

#ifdef SANITY_CHECK
#include <assert.h>
#endif 

colour_t
get_colour(char c)
{
  static char * index = COLOURS;

  char *p = strchr(index, c);

  if (!p){
    return COLOUR_NOT_FOUND;
  }

  return (p - index);
}

void*
xalloc(size_t n, size_t size)
{
  void *p = calloc(n,size);

  if (!p) {
    panicd("alloc failed for %zu elements * bytes", n * size);
  }

  return p;
}

uint32_t
atoui32(void *arg, uint32_t from, uint32_t to)
{
  const unsigned char *s = (unsigned char *) arg + from;
  unsigned char *t = (unsigned char *)(arg + to);

  uint32_t parsed = 0;

  #ifdef SANITY_CHECK 
  assert(from < to);
  #endif

  while (s < t) {
    #ifdef SANITY_CHECK 
    assert(*s >= '0' && *s <= '9');
    #endif

    parsed = (*s - '0') + parsed * 10;
    s ++;
  }

  return parsed;
}
