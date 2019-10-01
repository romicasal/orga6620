#ifndef __ANT_TYPES_H__
#define __ANT_TYPES_H__

#include <unistd.h>
#include <stdint.h>
#include "ant_constants.h"

#define COLOURS "RGBYNW"
#define COLOURS_LEN 6

#define COLOUR(d) COLOURS[(d)]

typedef enum colour {RED = CR, GREEN = CG, BLUE = CB, YELLOW = CY, BLACK = CN, WHITE = CW} colour_t;

typedef enum orientation {NORTH = ON, SOUTH = OS, EAST = OE, WEST = OW} orientation_t;

typedef enum rotation {LEFT = RL, RIGHT = RR} rotation_t;

typedef colour_t (*colour_fn)(void);
typedef rotation_t (*rule_fn)(colour_t);

typedef struct {
    uint32_t x, y;
    orientation_t o;
} ant_t;

#endif /* __ANT_TYPES_H__ */
