#ifndef __GRID_H__
#define __GRID_H__

#include "ant_types.h"

typedef struct {
  uint32_t rows, cols; 
  void (*set)(uint32_t x, uint32_t y, colour_t c);
  uint32_t (*get)(uint32_t x, uint32_t y);
} grid_handler_t; 

/* Create a grid and return the means to access it, along its properties */
grid_handler_t*
make_grid(const uint32_t,const uint32_t, const colour_t);

void
destroy_grid(void);

static grid_handler_t*
make_grid_handler(const uint32_t, const uint32_t);

#endif
