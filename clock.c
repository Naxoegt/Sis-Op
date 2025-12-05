#include "clock.h"
#include <stdlib.h>
#include <stdio.h>

static frame_entry *frames; 
static int n_frames;        
static int hand;           

void init_frame_table(int num) {
    n_frames = num;
    frames = (frame_entry *)malloc(n_frames * sizeof(frame_entry));
    hand = 0; // El reloj empieza 0

    for (int i = 0; i < n_frames; i++) {
        frames[i].use_bit = 0;
        frames[i].page_mapped = -1;
        frames[i].is_free = 1; // Todos libres al inicio
    }
}


void update_frame_access(int frame_number) {
    if (frame_number >= 0 && frame_number < n_frames) {
        frames[frame_number].use_bit = 1;
    }
}

int get_free_frame_or_victim(page_entry *page_table) {
    for (int i = 0; i < n_frames; i++) {
        if (frames[i].is_free) {
            frames[i].is_free = 0;
            frames[i].use_bit = 1; // Nueva oportunidad
            hand = (i + 1) % n_frames; // Mover la aguja
            return i;
        }
    }

    // 2. Si no hay vacíos, ejecutar algoritmo del RELOJ (Clock)
    while (1) {
        if (frames[hand].use_bit == 1) {
            // Si el bit es 1, le damos una segunda oportunidad (lo bajamos a 0)
            frames[hand].use_bit = 0;
            // Avanzamos la aguja circularmente
            hand = (hand + 1) % n_frames;
        } else {
            // Si el bit es 0, hemos encontrado a la víctima
            int victim_frame = hand;
            
            // IMPORTANTE: Invalidar la página virtual que vivía aquí antes
            int old_npv = frames[victim_frame].page_mapped;
            if (old_npv != -1) {
                page_table[old_npv].valid = 0; // La sacamos de la tabla de páginas
                // Nota: Aquí se simularía guardar a disco si fuera necesario
            }

            // Preparar el marco para la nueva página
            frames[victim_frame].use_bit = 1;            
            hand = (hand + 1) % n_frames;
            
            return victim_frame;
        }
    }
}

void update_frame_mapping(int frame, int npv) {
    frames[frame].page_mapped = npv;
}

void free_frame_table() {
    free(frames);
}

