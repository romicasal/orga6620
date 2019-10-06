#ifndef __ARTIST_ANT_H__
#define __ARTIST_ANT_H__

#include <unistd.h>
#include "grid.h"
#include "ant_types.h"
#include "builders.h"
#include "grid.h"

void* make_ant(uint32_t xini, uint32_t yini);

static void grid_out(grid_handler_t *grid);

#endif /* __ARTIST_ANT_H__ */

