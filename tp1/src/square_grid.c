#include <stdlib.h>

#include "ant_types.h"
#include "square_grid.h"
#include "builders.h"
#include "grid.h"

static uint32_t grid_rows, grid_cols;
static square_grid_t sq_grid;
static grid_handler_t handler;

static void 
paint_at(uint32_t x, uint32_t y, colour_t c)
{
  uint32_t offset = OFFSET_AT(x, y, grid_rows, grid_cols);
  sq_grid.grid[offset] = c;
}

static colour_t
colour_at(uint32_t x, uint32_t y)
{
  uint32_t offset = OFFSET_AT(x, y, grid_rows, grid_cols);
  return sq_grid.grid[offset];
}

void
destroy_grid(void)
{
  free(sq_grid.grid);
  sq_grid.grid = NULL;
}

grid_handler_t*
make_grid(const uint32_t rows, const uint32_t cols, const colour_t initial)
{
  uint32_t i;
  grid_rows = rows;
  grid_cols = cols;

  sq_grid.grid =  (colour_t *) xalloc (rows * cols, sizeof(colour_t));

  for (i = 0; i < rows * cols; i++) {
    sq_grid.grid[i] = initial;
  }

  return make_grid_handler(rows, cols);
}

static grid_handler_t*
make_grid_handler(const uint32_t rows, const uint32_t cols)
{
  handler.rows = rows;
  handler.cols = cols;

  handler.set = paint_at;
  handler.get = colour_at;

  return &handler;
}
