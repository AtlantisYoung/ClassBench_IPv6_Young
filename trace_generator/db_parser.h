#ifndef __DB_PARSER_H_
#define __DB_PARSER_H_
#include "FilterList.h"

// read and parse file from db_generator
int read_filters(FilterList *, FILE *);

// return standard IPv6 address format
std::string standard_ipv6(char ipv6_char[]);

#endif
