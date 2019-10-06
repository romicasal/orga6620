#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>

#include "ant_engine.h"
#include "builders.h"
#include "grid.h"

void*
paint(void *artist_ant, void *gridfn, colour_fn next_colour, rule_fn next_rotation, uint64_t iterations)
{
  ant_t* ant = (ant_t*) artist_ant;
  const grid_handler_t *grid_handler = (grid_handler_t *) gridfn;

  uint64_t i;
  uint32_t current_x, current_y;
  orientation_t current_o;
  colour_t current, next;
  rotation_t rule;

  for (i = 0; i < iterations ; i++) {
    /* Ant state */
    current_x = ant->x;
    current_y = ant->y;
    current_o = ant->o;

    current = grid_handler->get(current_x, current_y);

    rule = next_rotation(current);

    /* Paint */
    next = next_colour();
    grid_handler->set(current_x, current_y, next);

    /* Rotate */
    ant->o = new_orientation(current_o, rule);

    /* Move forward */
    ant = move_forward(ant, grid_handler->rows, grid_handler->cols);
  }

  return ant;
}
