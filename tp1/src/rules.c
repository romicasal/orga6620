#include "rules.h"
#include "builders.h"

static rotation_t mappings[COLOURS_LEN];

rule_fn 
make_rules(char *rule_spec, const size_t rule_spec_len,
	 char *palette_spec, const size_t palette_spec_len)
{
  rotation_t each_rotation;
  colour_t each_colour;
  unsigned int i;

#ifdef SANITY_CHECK
  if (rule_spec_len != palette_spec_len) {
    panic("rule and palette length mismatch");
  }
#endif

  for (i = 0; i < COLOURS_LEN; i++) {
    mappings[i] = INVALID_ROTATION;
  }

  for (i = 0; i < rule_spec_len; i++) {

    if (rule_spec[i] == 'L') {
      each_rotation = LEFT;
    }
    
    if (rule_spec[i] == 'R') {
      each_rotation = RIGHT;
    }
    each_colour = get_colour(palette_spec[i]);
		
    /* Sanity check */
    if (each_rotation != LEFT && each_rotation != RIGHT) {
      panicd("Faulting rotation %d", each_rotation);
    }

    if (COLOUR_NOT_FOUND == each_colour) {
      panicd("Faulting colour spec %c", palette_spec[i]);
    }

    /*
     * Shamelessly taking advantage of the fact that C enums
     * are integer types 
     */
     mappings[each_colour] = each_rotation;
  }

  return rule_for_colour;
}

/*
 * Needs to know about rules and the pallette, yet
 * C doesn't allow currying or nested functions (in a 
 * portable way)
 */
static rotation_t 
rule_for_colour(colour_t colour)
{

#ifdef SANITY_CHECK
  if (INVALID_ROTATION == mappings[colour]){
    panicd("Invalid rule for colour %c", COLOUR(colour));
   }
#endif
  return mappings[colour];
}
