#ifndef CLOCK_H
#define CLOCK_H

#include <stdint.h>
#include "pager.h"

typedef struct {
    int use_bit;      
    int page_mapped;  
    int is_free;      
} frame_entry;

void init_frame_table(int num_frames);
int get_free_frame_or_victim(page_entry *page_table);
void update_frame_access(int frame_number);
void free_frame_table();

#endif