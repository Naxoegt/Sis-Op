#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pager.h"
#include "clock.h"

#define MAX_LINE 256
// Asumimos un espacio de direcciones virtuales máximo de 2^32, 
// pero la tabla de páginas puede ser grande. 
// Para este simulador simple usaremos un tamaño fijo seguro suficiente para las trazas.
#define MAX_VPN 1048576 // 2^20 (suficiente para la mayoría de ejemplos simples)

int main(int argc, char *argv[]) {
    // Validación básica de argumentos [cite: 83]
    if (argc < 4) {
        fprintf(stderr, "Uso: %s <Nmarcos> <tamañomarco> [--verbose] <traza.txt>\n", argv[0]);
        return 1;
    }

    int n_frames = atoi(argv[1]);
    int page_size = atoi(argv[2]);
    char *filename = NULL;
    int verbose = 0;

    // Parsear argumentos opcionales
    if (strcmp(argv[3], "--verbose") == 0) {
        verbose = 1;
        if (argc > 4) filename = argv[4];
    } else {
        filename = argv[3];
    }

    if (!filename) {
        fprintf(stderr, "Error: Falta el archivo de traza.\n");
        return 1;
    }

    // Calcular bits de desplazamiento (b) usando función de Persona A
    int b = calc_b_from_page_size(page_size);
    if (b < 0) return 1;

    // Inicializar estructuras
    init_frame_table(n_frames);
    
    // Tabla de Páginas (Virtual). Inicializamos simple.
    // En una implementación real sería multinivel o hash, aquí un arreglo directo.
    page_entry *page_table = (page_entry *)calloc(MAX_VPN, sizeof(page_entry));

    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("Error abriendo archivo");
        return 1;
    }

    // Estadísticas
    unsigned long total_refs = 0;
    unsigned long page_faults = 0;

    char line[MAX_LINE];
    
    // Bucle Principal: Procesar Traza [cite: 62]
    while (fgets(line, sizeof(line), fp)) {
        // Limpiar línea y saltar vacías
        line[strcspn(line, "\r\n")] = 0;
        if (strlen(line) == 0) continue;

        total_refs++;
        
        uint32_t dv = parse_address(line);
        uint32_t npv = get_npv(dv, b);
        uint32_t offset = get_offset(dv, page_size);
        
        int hit = 0;
        int frame = -1;
        uint32_t df = 0;

        // Verificar HIT o FALLO [cite: 78, 79]
        if (page_table[npv].valid) {
            // HIT
            hit = 1;
            frame = page_table[npv].frame;
            update_frame_access(frame); // Actualizar bit de referencia (Reloj)
        } else {
            // FALLO (Page Fault)
            page_faults++;
            hit = 0;
            
            // Obtener marco (libre o víctima por Reloj)
            frame = get_free_frame_or_victim(page_table);
            
            // Actualizar tabla de páginas: mapear nuevo frame
            page_table[npv].frame = frame;
            page_table[npv].valid = 1;
            
            // Informar al gestor de marcos quién es el dueño (para invalidar a futuro si es víctima)
            // Necesitamos exponer esta función auxiliar o hacerlo manualmente en clock.c
            // Por simplicidad, asumimos que clock.c tiene acceso a actualizar su mapeo interno
            // (Ver función añadida abajo en clock.c extra)
             extern void update_frame_mapping(int frame, int npv); // Prototipo local
             update_frame_mapping(frame, npv);
        }

        // Calcular Dirección Física
        df = make_physical(frame, offset, b);

        // Salida Verbose [cite: 91]
        if (verbose) {
            verbose_print(dv, npv, offset, hit, frame, df);
        }
    }

    fclose(fp);
    
    // Reporte Final 
    printf("\n--- Resultados ---\n");
    printf("Archivo: %s\n", filename);
    printf("Memoria: %d marcos, %d bytes/página\n", n_frames, page_size);
    printf("Referencias totales: %lu\n", total_refs);
    printf("Fallos de página: %lu\n", page_faults);
    printf("Tasa de fallos: %.4f%%\n", (double)page_faults / total_refs * 100.0);

    // Limpieza
    free(page_table);
    free_frame_table();

    return 0;
}