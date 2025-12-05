#ifndef PAGER_H
#define PAGER_H

#include <stdint.h>

uint32_t parse_address(const char *s);
int calc_b_from_page_size(uint32_t page_size);
uint32_t get_npv(uint32_t dv, int b);
uint32_t get_offset(uint32_t dv, uint32_t page_size);
uint32_t make_physical(uint32_t frame, uint32_t offset, int b);
void verbose_print(uint32_t dv, uint32_t npv, uint32_t offset, int is_hit, int frame, uint32_t df);


typedef struct {
    int frame;
    int valid;
} page_entry;

#endif
