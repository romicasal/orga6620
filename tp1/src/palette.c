#include <stdlib.h>

#include "palette.h"
#include "builders.h"

static colour_t *palette;
static size_t palette_len;

static colour_t present[COLOURS_LEN];
static const unsigned int sentinel = COLOURS_LEN + 1;

void 
destroy_palette(void)
{
  free(palette);
  palette = NULL;
}

colour_fn
make_palette(const char *colour_spec, size_t palette_spec_len) 
{
  unsigned int i = 0;

  palette = xalloc(palette_spec_len , sizeof(colour_t));
  palette_len = palette_spec_len;

  colour_t each;

  for (i = 0; i < COLOURS_LEN; i++) {
    present[i] = sentinel;
  }

  for(i = 0; i < palette_spec_len ; i++) {
    each = get_colour(colour_spec[i]);

    if (COLOUR_NOT_FOUND == each) {
      panicd("Faulting colour spec %c", colour_spec[i]);
    }

    if (present[each] != sentinel) {
      panicd("Faulting colour spec, don't repeat values: %c", COLOUR(each));
    }

    present[each] = each;
    palette[i >> 1] = each;
  }
 
  return next_colour;
}

static colour_t
next_colour(void)
{
  static unsigned int current_index = 0;
  const unsigned int at = current_index++ % palette_len;

  return palette[at];
}

