#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "pager.h"  

int main() {
    char input[256];

    printf("Test direcciones virtuales ===\n");
    
    // Pedir tamanio pag
    printf("Ingresa el tamanio de pagina (ej: 4096): ");
    fgets(input, sizeof(input), stdin);
    uint32_t page_size = (uint32_t)strtoul(input, NULL, 10);

    int b = calc_b_from_page_size(page_size);
    if (b < 0) {
        printf("Tamaño de pagina inválido.\n");
        return 1;
    }

    printf("\nPAGE_SIZE = %u, entonces b = %d bits\n\n", page_size, b);

    // Bucle para probar direcciones
    while (1) {
        printf("Ingresa una dirección virtual DV (hex o decimal) o 'exit': ");

        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0'; // quitar salto de linea

        if (strcmp(input, "exit") == 0)
            break;

        // Convertir texto a num
        uint32_t dv = parse_address(input);

        // Obtener npv y offset
        uint32_t npv = get_npv(dv, b);
        uint32_t offset = get_offset(dv, page_size);

        printf("DV = %#010x (%u decimal)\n", dv, dv);
        printf("npv = %u\n", npv);
        printf("offset = %u (0x%x)\n\n", offset, offset);
    }

    return 0;
}

