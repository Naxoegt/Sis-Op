#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pager.h"
#include "clock.h"

// Se eligen estas constantes para tener limites razonables
#define MAX_LINE 256
#define MAX_VPN 1048576

int main(int argc, char *argv[]) {

    if (argc < 4) {
        fprintf(stderr, "Uso: %s <Nmarcos> <tamañomarco> [--verbose] <traza.txt>\n", argv[0]);
        return 1;
    }

    int n_frames = atoi(argv[1]);
    int page_size = atoi(argv[2]);
    char *filename = NULL;
    int verbose = 0;

    // Parseo del txt
    if (strcmp(argv[3], "--verbose") == 0) {
        verbose = 1;
        if (argc > 4) filename = argv[4];
    } else {
        filename = argv[3];
    }


    // Calcular bits de desplazamiento
    int b = calc_b_from_page_size(page_size);
    if (b < 0) return 1;
    init_frame_table(n_frames);
    
    // Tabla de paginas (parte vacia)
    page_entry *page_table = (page_entry *)calloc(MAX_VPN, sizeof(page_entry));

    FILE *fp = fopen(filename, "r");

    //Rwsultados de sim
    unsigned long total_refs = 0;
    unsigned long page_faults = 0;

    char line[MAX_LINE];
    
    // Funcion principal (bucle de referencias)
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strlen(line) == 0) continue;

        total_refs++;
        
        uint32_t dv = parse_address(line);
        uint32_t npv = get_npv(dv, b);
        uint32_t offset = get_offset(dv, page_size);
        
        int hit = 0;
        int frame = -1;
        uint32_t df = 0;

        // Si hay un fallo, se obtiene un marco libre o se aplica el algoritmo del reloj
        if (page_table[npv].valid) {
            hit = 1;
            frame = page_table[npv].frame;
            update_frame_access(frame); 
        } else {
            page_faults++;
            hit = 0;
            
            frame = get_free_frame_or_victim(page_table);
            page_table[npv].frame = frame;
            page_table[npv].valid = 1;
            
             extern void update_frame_mapping(int frame, int npv); 
             update_frame_mapping(frame, npv);
        }

        // Calcular direccion fisic
        df = make_physical(frame, offset, b);

        // Salida Verbose 
        if (verbose) {
            verbose_print(dv, npv, offset, hit, frame, df);
        }
    }

    fclose(fp);
    
    // Resultados (No mover, así se entiende mejor)
    printf("\n--- Resultados ---\n");
    printf("Archivo: %s\n", filename);
    printf("Memoria: %d marcos, %d bytes/página\n", n_frames, page_size);
    printf("Referencias totales: %lu\n", total_refs);
    printf("Fallos de página: %lu\n", page_faults);
    printf("Tasa de fallos: %.4f%%\n", (double)page_faults / total_refs * 100.0);

    free(page_table);
    free_frame_table();

    return 0;
}