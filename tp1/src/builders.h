#ifndef __BUILDERS_H__
#define __BUILDERS_H__

#include <stdlib.h>
#include <stdio.h>
#include "ant_types.h"

#define panic(s)       \
  do {                 \
    fprintf(stderr, "[%s %s %d]: %s\n", __FILE__, __FUNCTION__, __LINE__, s);\
    exit(1);            \
  } while(0);

#define panicd(format, d)       \
  do {                 \
    fprintf(stderr, "[%s %s %d]: "format"\n", __FILE__, __FUNCTION__, __LINE__, d);\
    exit(1);            \
  } while(0);


#define COLOUR_NOT_FOUND ((colour_t)-1)
#define INVALID_ROTATION ((rotation_t)-1)

#define spec_len(spec) (spec)

colour_t get_colour(char c);

void * xalloc(size_t n, size_t siz);

uint32_t atoui32(void *arg, uint32_t from, uint32_t to);
#endif /* __BUILDERS_H__ */
