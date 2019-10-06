#ifndef __PALETTE_H__
#define __PALETTE_H__ 

#include <string.h>
#include "ant_types.h"

colour_fn make_palette(const char *, size_t);

void destroy_palette(void);

static colour_t next_colour(void);

#endif /* __PALETTE_H__ */
