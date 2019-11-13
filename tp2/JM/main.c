/*
 * FILAS:
 * Cantidad de conjuntos = Bloques de cache / vias
 * Cantidad de conjuntos = 256 / 8 = 32
 * COLUMNAS:
 * FIFO = 1
 * Valid = 8x1
 * Dirty bit = 8x1
 * TAG =  8x1
 * Block = 8x1
 * Datos = 64 * 8 = 512
 * Total = 1 + 8 + 8 + 8 + 8 + 512 = 545
 * |F|...|V|D|T|B|DA|...
 * |1|...|1|1|1|1|64|... |1|1|1|64|....
 *  */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

#define CONJUNTOS 32
#define FIFOINDEX 0
#define VALIDDIRTYTAG 4
#define ANCHO 545
#define BLOCKSIZE 64
#define MMSIZE 65536
#define OFFSET 6
#define VIAS 8
#define SET 5
#define TAG 5
#define BITS 16
#define FILENAME "prueba5.mem"
float missRate = 0;
int accesosMemoria = 0;
float misses = 0;
float hits = 0;
unsigned int cache[CONJUNTOS][ANCHO];
unsigned int ram[MMSIZE];

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
unsigned int block_address(unsigned int address);
int main(void) {

    // printf("TAG: %d",  block_address(65472));
    // printf("%d \n",sizeof(char));

    char *line_buf = NULL;
    char bloque1[7];
    char bloque2[7];
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
            memcpy(bloque1,&line_buf[2], 7);
            int w;
            sscanf(bloque1, "%d", &w);

            char* token;
            char* rest = line_buf;
            int contar = 0;
            while ((token = strtok_r(rest, ",", &rest))) {
                if(contar == 1) {
                    memcpy(bloque2,token, 7);
                }
                contar++;
            }
            int v;
            sscanf(bloque2, "%d", &v );
            if(w >= MMSIZE || v > 255)
                printf("Dirección o valores no válidos: W %s \n", bloque1);
            else
                write_byte(w,v);
            break;
        case 'R':
            memcpy(bloque1,&line_buf[2], 7);
            int r;
            sscanf(bloque1, "%d", &r);

            if(r > MMSIZE)
                printf("Dirección no válida: %d\n", r);
            else
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

    /* Guardo el # de bloque */
    cache[set][find_blockIndex(way)+3] = BLOCKSIZE*blocknum;
}
void write_tomem(unsigned int blocknum, unsigned int way, unsigned int set) {
    for (int i = 0; i < BLOCKSIZE; i++) {
        ram[blocknum + i] = cache[set][find_blockIndex(way) + VALIDDIRTYTAG + i];
    }

}
unsigned int find_blockIndex(unsigned int way) {
    return 1 + way * (BLOCKSIZE + VALIDDIRTYTAG);
}
unsigned char read_byte(unsigned int address) {
    accesosMemoria++;
    int hit = 0;
    int via = 0;
    int viaOriginal = 0;
    int valid = 0;
    int tag = 0;
    int set = find_set(address);
    while (!hit && via < VIAS) {
        valid = cache[set][find_blockIndex(via)];
        tag = cache[set][find_blockIndex(via) + 2];
        if ((get_tag(address) == tag) && (valid)) {
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
        if(cache[set][find_blockIndex(via) + 1] == 1) {
            // printf("M Block: %d",cache[set][find_blockIndex(via) + 3]);
            write_tomem(cache[set][find_blockIndex(via) + 3], via, set);
        }


        /* Llevo el bloque a la Cache */
        read_tocache(block_address(address), via, set);
        /* Actualizo TAG */
        cache[set][find_blockIndex(via) + 2] = get_tag(address);

        via++;
        // Si llego al final, empiezo de nuevo
        if (via >= VIAS) {
            via = 0;
        }

        cache[set][0] = via;
        misses++;
        return cache[set][find_blockIndex(viaOriginal) + VALIDDIRTYTAG + get_offset(address)];
    } else {
        return cache[set][find_blockIndex(via) + VALIDDIRTYTAG + get_offset(address)];
    }
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
        if ((get_tag(address) == tag) && (valid)) { // Hit?

            /* Dirty bit = 1 */
            cache[set][find_blockIndex(via) + 1] = 1;

            /* Actualizo cache */
            cache[set][find_blockIndex(via) + VALIDDIRTYTAG + get_offset(address)] = value;
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
            write_tomem(cache[set][find_blockIndex(via) + 3], via, set);
        }
        /* Llevo el bloque a la Cache */
        read_tocache(block_address(address), via, set);

        /* Actualizo TAG */
        cache[set][find_blockIndex(via) + 2] = get_tag(address);
        /* Dirty bit = 1 */
        cache[set][find_blockIndex(via) + 1] = 1;

        /* Actualizo cache */
        cache[find_set(address)][find_blockIndex(via) + VALIDDIRTYTAG + get_offset(address)] = value;
        // Recalcular FIFO
        via++;
        // Si llego al final, empiezo de nuevo
        if (via >= VIAS) {
            via = 0;
        }

        cache[set][0] = via;

        misses++;
    }

}
float get_miss_rate() {
    return misses / accesosMemoria;
}
