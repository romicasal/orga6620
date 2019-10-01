#ifndef __SQUARE_GRID_H__
#define __SQUARE_GRID_H__

#include "artist_ant.h"

#ifdef USE_COL_MAJOR
#define OFFSET_AT(x,y,r,c)  ((x) + (c) * (y))
#else
#define OFFSET_AT(x,y,r,c) ((x) * (r) + (y))
#endif

typedef struct {
    colour_t *grid;
} square_grid_t;

#endif /* __SQUARE_GRID_H__ */
