#ifndef __TRACE_TOOLS_H_
#define __TRACE_TOOLS_H_
#include "flist.h"
#include "FilterList.h"

int header_gen(int d, FilterList *filters, FILE *fp, float a, float b, int scale);
void random_corner(int RandFilt, FilterList *filters, struct header *new_hdr);
int my_pareto(float a, float b);
int linear_search(FilterList *filters,struct header *hdr);
uint128_t high(uint128_t addr, int len);
uint128_t low(uint128_t addr, int len);

#endif
