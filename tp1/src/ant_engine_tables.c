#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>

#include "ant_engine.h"

typedef void (*step_fn)(ant_t*, uint32_t);

void step_west(ant_t *ant, uint32_t width) {
  adjust(&ant->x, ant->x - 1, width);
}

void step_east(ant_t *ant, uint32_t width) {
  adjust(&ant->x, ant->x + 1, width);
}

void step_north(ant_t *ant, uint32_t height) {
  adjust(&ant->y, ant->y - 1, height);
}

void step_south(ant_t *ant, uint32_t height) {
  adjust(&ant->y, ant->y + 1, height);
}

static step_fn allowed_forward[4] = {step_north, step_south, step_east, step_west};

#ifdef USE_GLOBAL_GRID_DIMS
extern uint32_t relevant_bounds[4]; 
#endif /* USE_GLOBAL_GRID_DIMS */

ant_t*
move_forward(ant_t *ant, const uint32_t width, const uint32_t height)
{
#ifndef USE_GLOBAL_GRID_DIMS
  uint32_t relevant_bounds[4] = {height, height, width, width};
#endif /*USE_GLOBAL_GRID_DIMS */

  /* handle infinite grid */
  register uint32_t bound = relevant_bounds[ant->o];

  /* retrieve the correct function */
  register step_fn go_forward = allowed_forward[ant->o];

  /* do step */
  go_forward(ant, bound);

  return ant;
}

orientation_t 
new_orientation(orientation_t orientation, rotation_t rule)
{
  static orientation_t rotation_rules[4][2] = {
    {WEST,EAST},    /* north */
    {EAST,WEST},    /* south */
    {NORTH, SOUTH}, /* east */
    {SOUTH,NORTH}   /* west */
  };

  return rotation_rules[orientation][rule];
}
