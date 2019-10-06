#ifndef __ANT_ENGINE_H__
#define __ANT_ENGINE_H__

#include <stdio.h>
#include <unistd.h>

#include "ant_types.h"
#include "builders.h"

#define adjust(p, v, limit) \
  do{                       \
    *(p) = (v) % (limit);   \
  } while(0)             
 
void *paint(void *artist_ant, void *gridfn, colour_fn next_colour, rule_fn rotation_for_colour, uint64_t iterations);

orientation_t new_orientation(orientation_t orientation, rotation_t rule);

ant_t* move_forward(ant_t* ant, uint32_t width, uint32_t height);


#endif /* __ANT_ENGINE_H__ */

