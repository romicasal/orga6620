#include <stdio.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <limits.h>

#include "ant_engine.h"
#include "artist_ant.h"
#include "builders.h"
#include "grid.h"
#include "palette.h"
#include "rules.h"
#include "ant_types.h"

static FILE *outfile; 

#ifdef USE_GLOBAL_GRID_DIMS
uint32_t relevant_bounds[4];
#endif /* USE_GLOBAL_GRID_DIMS */

static void
show_warn(char *p)
{
  fprintf(stderr, " %s\n", p);
}

static void
show_example(char *p)
{
  fprintf(stderr, "Example: \n");  
  fprintf(stderr,"\t%s -g 2x2 -p RGB -r LLL -t 5\n", p);
}
static void
show_help(char *msg) {
  fprintf(stderr, "  %s -g <grid_spec> -p <colour_spec> -r <rule_spec> -t <n>\n", msg);
  fprintf(stderr, "  -g --grid: wxh\n");
  fprintf(stderr, "  -p --palette: Combination of RGBYNW characters\n");
  fprintf(stderr, "  -r --rules: Combination of LR characters\n");
  fprintf(stderr, "  -t --times: Iterations. If negative, it's complement will be used.\n");
  fprintf(stderr, "  -o --outfile: output file. Defaults to stdout.\n");
  fprintf(stderr, "  -h --help: Print this message and exit\n");
  fprintf(stderr, "  -v --verbose: Version number\n");
  
  fprintf(stderr, "\n");
  fprintf(stderr, "Compile with -DSANITY_CHECK to enable runtime checks\n");
  fprintf(stderr, "Compile with -DUSE_TABLES to execute ant operations in separate functions\n");
  fprintf(stderr, "Compile with -DUSE_COL_MAJOR to traverse the grid in column-major order\n");
  show_example(msg);

}

static void
show_version()
{
  fprintf(stderr, "v1.0\n");
  #ifdef USE_TABLES
  fprintf(stderr, "USE_TABLES ON\n");
  #endif
  #ifdef SANITY_CHECK
  fprintf(stderr, "SANITY_CHECK ON\n");
  #endif
  #ifdef USE_COL_MAJOR
  fprintf(stderr, "COL_MAJOR ON \n");
  #endif
}

#define check_required(w, cond)      \
  do {                               \
    if (!(cond)) {                   \
      show_warn((w));                \
      show_help(argv[0]);            \
      exit(1);                       \
    }                                \
  } while(0)

int
main(int argc, char **argv)
{
  static struct option long_options[] = {
    {"grid",  1, 0, 'g'},
    {"palette", 1, 0, 'p'},
    {"rules",  1, 0, 'r'},
    {"times",  1, 0, 't'},
    {"outfile", 1, 0, 'o'},
    {"help", 0, 0, 'h'},
    {"version", 0, 0, 'v'},
    {0, 0, 0, 0}
  };

  uint64_t iterations = 0 ;

  int long_index = 0;

  char *rule_spec = 0 , *grid_spec = 0, *colour_spec = 0;
  char *end;
  
  uint32_t grid_width, grid_height;
  size_t colour_spec_len, rule_spec_len, grid_spec_len;

  int opt, s;
  char *dim_separator;

  ant_t *artist_ant;
  grid_handler_t *grid;

  char initial_colour_spec;
  colour_t initial;
  colour_fn next_colour_fn;
  rule_fn rules;

  outfile = stdout;
  /* Parse arguments */
  while ((opt = getopt_long(argc, argv, "g:p:r:t:o:hv", long_options, &long_index)) != -1) {

    switch(opt) {
      case 'o': /* outfile */
        if (strcmp(optarg, "-") != 0) {
          errno = 0;
          outfile = fopen(optarg, "w");

          if (!outfile) {
            fprintf(stderr, "Unable to open outfile %s: errno %d\n", optarg, errno);
            exit(2);
          }
        }
        break;
      case 'g': /* grid */
        dim_separator = strchr(optarg, 'x');
        assert(dim_separator != NULL);

        s = (unsigned int) (dim_separator - optarg);
        grid_spec_len = strlen(optarg);

        grid_width = atoui32(optarg, 0, s);
        grid_height = atoui32(optarg, s+1, grid_spec_len);

#ifdef USE_GLOBAL_GRID_DIMS
        relevant_bounds[0] = grid_height;
        relevant_bounds[1] = grid_height;
        relevant_bounds[2] = grid_width;
        relevant_bounds[3] = grid_width;
#endif /* USE_GLOBAL_GRID_DIMS */

        break;
      case 'p': /* palette */
        initial = get_colour(optarg[0]);
        colour_spec_len = strlen(optarg);
        colour_spec = xalloc(1, colour_spec_len);
        memcpy(colour_spec, optarg, colour_spec_len);

        break;

      case 'r': /* rules */
       rule_spec_len = strlen(optarg);
       rule_spec = xalloc(1, rule_spec_len);
       memcpy(rule_spec, optarg, rule_spec_len);

       break;

      case 't': /* times */
        errno = 0;
        iterations = strtoull(optarg, &end, 10);

        if (ULLONG_MAX == iterations && ERANGE == errno) {
          show_warn("<times> arg out of range.");
          exit(1);
        }

        if (optarg == end) {
          show_warn("Can't parse <times> arg. No digits found");
          exit(1);
        }

        break;
      case 'h': /* help */
        show_help(argv[0]);
        exit(0);
        break;

      case 'v': /* version */
        show_version();
        exit(0);
        break;

      default:
        fprintf(stderr, "Unrecognized option %c\n", opt );
        show_help(argv[0]);
        exit(1);
    }
  }
  /* Check arguments */
  check_required("Grid spec (h) is required", grid_height > 0);
  check_required("Grid spec (w) is required", grid_width > 0);
  check_required("Iterations is required", iterations > 0);

  check_required("Rule spec is required", rule_spec);
  check_required("Colour spec is required", colour_spec);
  check_required("Rule and colour length should match", rule_spec_len == colour_spec_len );

  /* Get stuff in place */
  grid = make_grid(grid_width, grid_height, initial);

  /* Our palette won't consider the initial colour */
  next_colour_fn = make_palette(colour_spec, colour_spec_len);
  rules = make_rules(rule_spec, rule_spec_len, colour_spec, colour_spec_len);

  artist_ant = make_ant(grid_width >> 1, grid_height >> 1);
  
  /* Start the show */
  paint(artist_ant, grid, next_colour_fn, rules, iterations);

  grid_out(grid);

  free(artist_ant);
  artist_ant = NULL;
  
  destroy_palette();
  destroy_grid();
  
  return 0;
}

void*
make_ant(uint32_t xini, uint32_t yini)
{
  ant_t *ant = xalloc(1, sizeof(ant_t));
  ant->x = xini;
  ant->y = yini;
  ant->o = NORTH;

  return ant;
}

static void 
grid_out(grid_handler_t* grid)
{
  colour_t c;
  unsigned int grid_width = grid ->rows; 
  unsigned int grid_height = grid -> cols;

  fprintf(outfile, "P3\n");
  fprintf(outfile, "%d %d\n", grid_width, grid_height);
  fprintf(outfile, "255\n");

#ifdef USE_COL_MAJOR
  for (unsigned int j = 0;  j < grid_height; j++) {
    for (unsigned int i = 0;  i < grid_width; i++) {
#else
  for (unsigned int i = 0;  i < grid_width; i++) {
    for (unsigned int j = 0;  j < grid_height; j++) {
#endif
      c = grid->get(i,j);

      switch(c) {
      case RED:
        fprintf(outfile, "%d %-3d %-3d ", 255, 0, 0);
        break;
      case GREEN:
        fprintf(outfile, "%-3d %d %-3d ", 0, 255, 0);
        break;
      case BLUE:
        fprintf(outfile, "%-3d %-3d %d ", 0, 0, 255);
        break;
      case WHITE:
        fprintf(outfile, "%d %d %d ", 255, 255, 255);
        break;
      case YELLOW:
        fprintf(outfile, "%d %d %-3d ", 255, 255, 0);
        break;
      case BLACK:
        fprintf(outfile, "%-3d %-3d %-3d ", 0, 0, 0);
        break;
      default:
        fprintf(stderr, "Invalid grid colour: %c\n", c);
        exit(2);
      }
    }
    fprintf(outfile, "\n");
  }
}


