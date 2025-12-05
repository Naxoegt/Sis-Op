#include "pager.h"
#include <stdlib.h>
#include <stdio.h>

uint32_t parse_address(const char *s) {
    if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
        return (uint32_t)strtoul(s, NULL, 16);
    }
    return (uint32_t)strtoul(s, NULL, 10);
}

int calc_b_from_page_size(uint32_t page_size) {
    if (page_size == 0 || (page_size & (page_size - 1)) != 0) {
        fprintf(stderr, "page_size must be power of 2\n");
        return -1;
    }

    int b = 0;
    while ((page_size >>= 1) != 0) {
        b++;
    }
    return b;
}

uint32_t get_offset(uint32_t dv, uint32_t page_size) {
    return dv & (page_size - 1);
}

uint32_t get_npv(uint32_t dv, int b) {
    return dv >> b;
}

uint32_t make_physical(uint32_t frame, uint32_t offset, int b) {
    return (frame << b) | offset;
}

void verbose_print(uint32_t dv, uint32_t npv, uint32_t offset, int is_hit, int frame, uint32_t df){
    printf("DV=%#010x npv=%u offset=%u %s marco=%d DF=%#010x\n", dv, npv, offset, is_hit ? "HIT" : "FALLO", frame, df);
}
