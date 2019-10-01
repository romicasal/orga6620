#include <stdio.h>
#include <assert.h>

#include "grid.h"
#include "artist_ant.h"
#include "palette.h"
#include "rules.h"

#define all_passed() printf("[%s] All tests OK\n", __FILE__);

#ifndef TEST
#error "Use -DTEST and a specific test to run"
#endif

#ifdef GRID_TEST
int
main(void)
{
  grid_handler_t *handler = make_grid(2,2, RED);  

	assert(handler->width == 2);
	assert(handler->height == 2);
	handler->set(0,0,BLUE);

	assert(handler->get(0,0) == BLUE);
	assert(handler->get(0,1) == RED);
	assert(handler->get(1,0) == RED);
	assert(handler->get(1,1) == RED);

	all_passed();
  return 0;
}
#endif /* GRID_TEST */

#ifdef PALETTE_TEST

int
main(void)
{
  const char *spec = "R|G|B";
  colour_fn next_colour = make_palette(spec, 5);

	unsigned int i =0 ;
	
	for (; i < 10; i++){
	  assert(next_colour() == RED);
	  assert(next_colour() == GREEN);
	  assert(next_colour() == BLUE);
	}

	all_passed();
}

#endif /* PALETTE_TEST */

#ifdef MAPPING_TEST
int
main(void)
{
 char *colours = "R|G|B";
 char *rules = "L|R|L";

 rule_fn get= make_rules(rules, 5, colours, 5);

 assert(get(BLUE) == LEFT);
 assert(get(RED) == LEFT);
 assert(get(GREEN) == RIGHT);

 all_passed();
}
#endif /* MAPPING_TEST */
