CFLAGS= -mno-mips16 -mgp32 -mfp32 -gpubnames  -mlong32 -mips1 -mabicalls -mlong-calls -mframe-header-opt -march=r2k 

OBJS= tp1_if_asm tp1_tables_asm

all: tp1_if tp1_tables tp1_tables_pf tp1_if_pf

prof: tp1_if_pf tp1_tables_pf

col_major:tp1_if_col_major tp1_tables_col_major

global_dims: tp1_tables_global_dims

tp1_if: square_grid.c builders.c palette.c rules.c square_grid.c ant_engine.c ant_engine_jumps.c artist_ant.c 
	gcc -g $^ -o $@

tp1_tables: square_grid.c builders.c palette.c rules.c square_grid.c ant_engine.c ant_engine_tables.c artist_ant.c 
	gcc -g -DUSE_TABLES -DUSE_GLOBAL_GRID_DIMS $^ -o $@

tp1_if_pf: square_grid.c builders.c palette.c rules.c square_grid.c ant_engine.c ant_engine_jumps.c artist_ant.c 
	gcc -no-pie -pg -g -DSANITY_CHECK $^ -o $@

tp1_tables_pf: square_grid.c builders.c palette.c rules.c square_grid.c ant_engine.c ant_engine_tables.c artist_ant.c
	gcc -no-pie -pg -g -DUSE_TABLES $^ -o $@

tp1_if_col_major: square_grid.c builders.c palette.c rules.c square_grid.c ant_engine.c ant_engine_jumps.c artist_ant.c 
	gcc -no-pie -pg -g -DUSE_COL_MAJOR $^ -o $@

tp1_tables_col_major: square_grid.c builders.c palette.c rules.c square_grid.c ant_engine.c ant_engine_tables.c artist_ant.c 
	gcc -no-pie -pg -g -DUSE_TABLES -DUSE_COL_MAJOR $^ -o $@

tp1_tables_global_dims: square_grid.c builders.c palette.c rules.c square_grid.c ant_engine.c ant_engine_tables.c artist_ant.c 
	gcc -g -DUSE_TABLES -DUSE_GLOBAL_GRID_DIMS $^ -o $@

tp1_tables_asm: square_grid.c builders.c palette.c rules.c square_grid.c ant_engine.c ant_engine_tables.S asm-helpers.c artist_ant.c 
	gcc -DUSE_GLOBAL_GRID_DIMS -g $(CFLAGS) $^ -o $@

tp1_if_asm: square_grid.c builders.c palette.c rules.c square_grid.c ant_engine.c ant_engine_jumps.S asm-helpers.c artist_ant.c 
	gcc -DUSE_GLOBAL_GRID_DIMS -g $(CFLAGS) $^ -o $@
clean:
	rm tp1_*
