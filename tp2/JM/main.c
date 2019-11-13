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
int accesosMemoria = 0;
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


    // printf("%d \n",sizeof(char));

    char *line_buf = NULL;
    char bloque1[5];
    char bloque2[5];
    size_t line_buf_size = 0;
    int line_count = 0;
    ssize_t line_size;
    FILE *fp = fopen(FILENAME, "r");
    if (!fp) {
        fprintf(stderr, "Error opening file '%s'\n", FILENAME);
        return EXIT_FAILURE;
    }

    line_size = getline(&line_buf, &line_buf_size, fp);

    init();

    while (line_size >= 0) {
        line_count++;
        switch(line_buf[0]) {
        case 'F':
            //printf("FLUSH \n");
            init();
            break;
        case 'W':
            memcpy(bloque1,&line_buf[2], 5);
            int w;
            sscanf(bloque1, "%d", &w);

            char* token;
            char* rest = line_buf;
            int contar = 0;
            while ((token = strtok_r(rest, ",", &rest))) {
                if(contar == 1) {
                    memcpy(bloque2,token, 5);
                }
                contar++;
            }
            int v;
            sscanf(bloque2, "%d", &v );
            printf("W -> Dirección: %d \t Valor: %d\n ",w,v);
            write_byte(w,v);
            break;
        case 'R':
            memcpy(bloque1,&line_buf[2], 5);
            bloque1[5] = '\0';
            int r;
            sscanf(bloque1, "%d", &r);
            printf("R -> Dirección: %d  ", r);
            printf("Resultado: %d \n",read_byte(r));
            break;
        case 'M':
            printf("MR: %0.2f%% \n", get_miss_rate()*100);
            break;
        default:
            break;
        }


        line_size = getline(&line_buf, &line_buf_size, fp);
    }

    free(line_buf);
    line_buf = NULL;

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
    /* llevo el bloque desde RAM hasta cache */
    for (int i = 0; i < BLOCKSIZE; i++) {
        cache[set][find_blockIndex(way) + VALIDDIRTYTAG + i] = ram[blocknum * BLOCKSIZE + i];
    }
    /* Valid = 1 */
    cache[set][find_blockIndex(way)] = 1;
}
void write_tomem(unsigned int blocknum, unsigned int way, unsigned int set) {
    for (int i = 0; i < BLOCKSIZE; i++) {
        ram[blocknum * BLOCKSIZE + i] = cache[set][find_blockIndex(way) + VALIDDIRTYTAG + i];
    }
    printf("Block: %d Via: %d \n", blocknum, way);
    printf("Cache: %d  \n", cache[set][find_blockIndex(way) + VALIDDIRTYTAG]);
    printf("RAM: %d  \n", ram[blocknum * BLOCKSIZE]);
}
unsigned int find_blockIndex(unsigned int way) {
    return 1 + way * (BLOCKSIZE + VALIDDIRTYTAG);
}
unsigned char read_byte(unsigned int address) {
    // printf("TAG: %d SET: %d \n",get_tag(address),find_set(address));
        accesosMemoria++;
    int hit = 0;
    int via = 0;
    int viaOriginal = 0;
    int valid = 0;
    int tag = 0;
    int set = find_set(address);
    while (!hit && via < VIAS) {
        // printf("Igual? %d = %d  via: %d Valid: %d\n",get_tag(address), cache[find_set(address)][find_blockIndex(via) + 2], via,cache[find_set(address)][find_blockIndex(via)]);
        valid = cache[set][find_blockIndex(via)];
        tag = cache[set][find_blockIndex(via) + 2];
        if ((get_tag(address) == tag) && (valid)) {
           // printf("\t HIT ");
            hit = 1;
            hits++;
            break;
        }
        via++;

    };
    if (!hit) {
        via = cache[set][0];
        viaOriginal = via;
        /* Si Dirty bit = 1, escribo en memoria */
        //printf("Dirty bit: %d \n",cache[set][find_blockIndex(via) + 1] );
        if(cache[set][find_blockIndex(via) + 1] == 1) {
            write_tomem(block_address(address), via, set);
        }
        cache[set][0] = via;
        /* Llevo el bloque a la Cache */
        read_tocache(block_address(address), via, set);
        /* Actualizo TAG */
        cache[set][find_blockIndex(via) + 2] = get_tag(address);

        via++;
        // Si llego al final, empiezo de nuevo
        if (via == VIAS) {
            via = 0;
        }

        cache[set][0] = via;
        misses++;
        return cache[set][find_blockIndex(viaOriginal) + VALIDDIRTYTAG + get_offset(address)];
    }
    /* Actualizo cache */
    return cache[set][find_blockIndex(via) + VALIDDIRTYTAG + get_offset(address)];
}

void write_byte(unsigned int address, unsigned char value) {
        accesosMemoria++;
    int hit = 0;
    int via = 0;
    int valid = 0;
    int tag = 0;
    int set = find_set(address);
    while (!hit && via < VIAS) {
        valid = cache[set][find_blockIndex(via)];
        tag = cache[set][find_blockIndex(via) + 2];
        if ((get_tag(address) == tag) && (valid)) {
            /* Dirty bit = 1 */
            cache[find_set(address)][find_blockIndex(via) + 1] = 1;
            /* Actualizo cache */
            cache[find_set(address)][find_blockIndex(via) + VALIDDIRTYTAG + get_offset(address)] = value;
            //printf("\t HIT ");
            hit = 1;
            hits++;
            break;
        }
        via++;

    }
    if (!hit) {
        via = cache[set][0];

        /* Si Dirty bit = 1, escribo en memoria */
        if(cache[set][find_blockIndex(via) + 1] == 1) {
            write_tomem(block_address(address), via, set);
            printf("Dirty bit = 1\n");
        }

        /* Llevo el bloque a la Cache */
        read_tocache(block_address(address), via, set);
        /* Actualizo TAG */
        cache[set][find_blockIndex(via) + 2] = get_tag(address);

        /* Actualizo cache */
        cache[find_set(address)][find_blockIndex(via) + VALIDDIRTYTAG + get_offset(address)] = value;
        // Recalcular FIFO
        via++;
        // Si llego al final, empiezo de nuevo
        if (via == VIAS) {
            via = 0;
        }

        cache[set][0] = via;

        misses++;
    }



}
float get_miss_rate() {
    return misses / accesosMemoria;
}
