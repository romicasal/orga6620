/*
 ============================================================================
 Name        : MemoriaCache.c
 Author      : delpinor
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
/*
 * FILAS:
 * Cantidad de conjuntos = Bloques de cache / vias
 * Cantidad de conjuntos = 256 / 8 = 32
 * COLUMNAS:
 * FIFO = 1
 * Valid = 8x1
 * Dirty bit = 8x1
 * TAG =  8x1
 * Datos = 64 * 8 = 512
 * Total = 8 + 8 + 1 + 8 + 512 = 537
 * |F|...|v|D|T|DA|....
 * |1|...|1|1|1|64|... |1|1|1|64|....
 *  */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

#define CONJUNTOS 32
#define FIFOINDEX 0
#define VALIDDIRTYTAG 3
#define BASEDATAINDEX 4
#define ANCHO 537
#define BLOCKSIZE 64
#define MMSIZE 65536
#define OFFSET 6
#define VIAS 8
#define SET 5
#define TAG 5
#define BITS 16
#define FILENAME "prueba1.mem"
float missRate = 0;
float accesosMemoria = 0;
float misses = 0;
float hits = 0;
char cache[CONJUNTOS][ANCHO];
char ram[MMSIZE];

void init();
unsigned int get_offset(unsigned int address);
unsigned int get_tag(unsigned int address);
unsigned int find_set(unsigned int address);
unsigned int select_oldest(unsigned int setnum);
void read_tocache(unsigned int blocknum, unsigned int way, unsigned int set);
unsigned int find_blockIndex(unsigned int way);
void write_tomem(unsigned int blocknum, unsigned int way, unsigned int set);
unsigned char read_byte(unsigned int address);
void write_byte(unsigned int address, unsigned char value);
float get_miss_rate();
int main(void) {

      /* Open the file for reading */

  char *line_buf = NULL;
  char bloque1[5];
  char bloque2[5];
  size_t line_buf_size = 0;
  int line_count = 0;
  ssize_t line_size;
  FILE *fp = fopen(FILENAME, "r");
  if (!fp)
  {
    fprintf(stderr, "Error opening file '%s'\n", FILENAME);
    return EXIT_FAILURE;
  }

  /* Get the first line of the file. */
  line_size = getline(&line_buf, &line_buf_size, fp);

  /* Loop through until we are done with the file. */
  while (line_size >= 0)
  {
    /* Increment our line count */
    line_count++;


    printf("contents: %c \n", line_buf[0]);
    switch(line_buf[0]){
    case 'F':
        init();
        break;
    case 'w':
        break;
    case 'R':
        memcpy(bloque1,&line_buf[2], 5);
        bloque1[5] = '\0';
        int i;
        sscanf(bloque1, "%d", &i);
        printf("READ: %d \n", i);
        read_byte(i);
        break;
    case 'M':
        break;
    default:
        break;
    }

    /* Show the line details */
    //printf("line[%06d]: chars=%06zd, buf size=%06zu, contents: %s", line_count,line_size, line_buf_size, line_buf);

    /* Get the next line */
    line_size = getline(&line_buf, &line_buf_size, fp);
  }

  /* Free the allocated line buffer */
 free(line_buf);
 line_buf = NULL;

  /* Close the file now that we are done with it */
  fclose(fp);

	return 0;
}
void init() {
	memset(ram, 0, sizeof(ram));
	memset(cache, 0, sizeof(cache[0][0]) * CONJUNTOS * ANCHO);
	missRate = 0;
}
unsigned int get_offset(unsigned int address) {
	return address % BLOCKSIZE;
}
unsigned int block_address(unsigned int address) {
	return address / BLOCKSIZE;
}
unsigned int get_tag(unsigned int address) {
	int bin[BITS] = { 0 };
	for (int i = 15; i >= 0; i--) {
		bin[i] = address % 2;
		address = address / 2;
	}
	int etiqueta = 0;
	for (int j = 0; j < 5; j++) {
		etiqueta = etiqueta + bin[4 - j] * pow(2, j);
	}
	return etiqueta;
}

unsigned int find_set(unsigned int address) {
	return block_address(address) % CONJUNTOS;
}
unsigned int select_oldest(unsigned int setnum) {
	return cache[setnum][FIFOINDEX];
}
void read_tocache(unsigned int blocknum, unsigned int way, unsigned int set) {
    /* Si Dirty bit = 1, escribo en memoria */
    if(cache[set][find_blockIndex(way) + 1] == 1){
        write_tomem(blocknum, way, set);
    }
    /* llevo el bloque desde RAM hasta cache */
	for (int i = 0; i < BLOCKSIZE; i++) {
		cache[set][find_blockIndex(way) + VALIDDIRTYTAG + i] = ram[blocknum * BLOCKSIZE + i];
	}
	/* Valid = 1 */
	cache[set][find_blockIndex(way)] = 1;
}
void write_tomem(unsigned int blocknum, unsigned int way, unsigned int set){
    for (int i = 0; i < BLOCKSIZE; i++) {
		ram[blocknum * BLOCKSIZE + i] = cache[set][find_blockIndex(way) + VALIDDIRTYTAG + i];
	}
}
unsigned int find_blockIndex(unsigned int way) {
	return 1 + way * (BLOCKSIZE + VALIDDIRTYTAG);
}
unsigned char read_byte(unsigned int address) {
	accesosMemoria++;
	int hit = 0;
	int via = 0;
	while (!hit && via < VIAS) {
		if (get_tag(address) == cache[find_set(address)][find_blockIndex(via) + 2]) {
			hit = 1;
			hits++;
			break;
		}
		via++;
	};
	if (!hit) {
		/* Llevo el bloque a Memoria */
		read_tocache(block_address(address), cache[find_set(address)][0], find_set(address));
		// Via desde donde se devuelve el Dato.
		via = cache[find_set(address)][0];
		// Recalcular FIFO
		cache[find_set(address)][0] = 1 + cache[find_set(address)][0];
		// Si llego al final, empiezo de nuevo
		if (cache[find_set(address)][0] == VIAS) {
			cache[find_set(address)][0] = 0;
		}
		misses++;
	}
	/* Actualizo cache */
	return cache[find_set(address)][find_blockIndex(via) + 3 + get_offset(address)];
}

void write_byte(unsigned int address, unsigned char value) {
	accesosMemoria++;
	int hit = 0;
	int via = 0;
	while (!hit && via < VIAS) {
		if (get_tag(address) == cache[find_set(address)][find_blockIndex(via) + 2]) {
			hit = 1;
			hits++;
			break;
		}
		via++;
	};
	if (!hit) {
		/* Llevo el bloque a Memoria */
		read_tocache(block_address(address), cache[find_set(address)][0],find_set(address));
		// Via desde donde se guarda el Dato.
		via = cache[find_set(address)][0];
		/* Dirty bit = 1 */
		cache[find_set(address)][find_blockIndex(via) + 1] = 1;
		// Recalcular FIFO
		cache[find_set(address)][0] = 1 + cache[find_set(address)][0];
		// Si llego al final, empiezo de nuevo
		if (cache[find_set(address)][0] == VIAS) {
			cache[find_set(address)][0] = 0;
		}
		misses++;
	}
	/* Actualizo cache */
	cache[find_set(address)][find_blockIndex(via) + 3 + get_offset(address)] = value;
}
float get_miss_rate() {
	return misses / accesosMemoria;
}
