#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>

#include "ant_engine.h"

static orientation_t
decide(rotation_t d, orientation_t go_left, orientation_t go_right)
{
  return d == RIGHT ? go_right : go_left;
}

orientation_t 
new_orientation(orientation_t orientation, rotation_t rule)
{
  orientation_t updated_orientation;

  switch(orientation) {
  case NORTH:
    updated_orientation = decide(rule, WEST, EAST);
    break;
  case SOUTH:
    updated_orientation = decide(rule, EAST, WEST);
    break;
  case EAST:
    updated_orientation = decide(rule, NORTH, SOUTH);
    break;
  case WEST:
    updated_orientation = decide(rule, SOUTH, NORTH);
    break;
  default:
    panicd("Unknown orientation %d", orientation);
  }

  return updated_orientation;
}

ant_t*
move_forward(ant_t *ant, uint32_t width, uint32_t height) 
{
   
  switch(ant->o) { 
    case NORTH:
      adjust(&ant->y, ant->y - 1, height);
      break;
    case SOUTH:
      adjust(&ant->y, ant->y + 1, height);
      break;
    case EAST:
      adjust(&ant->x, ant->x + 1, width);
      break;
    case WEST:
      adjust(&ant->x, ant->x - 1, width);
      break;
    default:
      panicd("Unknown orientation %d", ant->o);
  }

  return ant;
}

