#include "clock.h"
#include <stdlib.h>
#include <stdio.h>

static frame_entry *frames; 
static int n_frames;        
static int hand;           


//Inicia la tabla con los marcos de pagina
void init_frame_table(int num) {
    n_frames = num;
    frames = (frame_entry *)malloc(n_frames * sizeof(frame_entry));
    hand = 0; 

    for (int i = 0; i < n_frames; i++) {
        frames[i].use_bit = 0;
        frames[i].page_mapped = -1;
        frames[i].is_free = 1; // Todo libre al inicio
    }
}


void update_frame_access(int frame_number) {
    if (frame_number >= 0 && frame_number < n_frames) {
        frames[frame_number].use_bit = 1;
    }
}

//Si no hay marcos libres, aplica el algoritmo del reloj para encontrar una "victima"
int get_free_frame_or_victim(page_entry *page_table) {
    for (int i = 0; i < n_frames; i++) {
        if (frames[i].is_free) {
            frames[i].is_free = 0;
            frames[i].use_bit = 1; 
            hand = (i + 1) % n_frames; 
            return i;
        }
    }
    //Esencia del algoritmo del reloj
    while (1) {
        if (frames[hand].use_bit == 1) {
            frames[hand].use_bit = 0;
            hand = (hand + 1) % n_frames;
        } else {
            int victim_frame = hand;
            
            int old_npv = frames[victim_frame].page_mapped;
            if (old_npv != -1) {
                page_table[old_npv].valid = 0;
            }
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

