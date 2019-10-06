#ifndef __RULES_H__
#define __RULES_H__

#include <string.h>
#include "ant_types.h"

rule_fn 
make_rules(char *rule_spec, const size_t rule_spec_len,
           char *palette_spec, const size_t pallete_spec_len);

static rotation_t rule_for_colour(colour_t);

#endif /* __RULES_H__ */
