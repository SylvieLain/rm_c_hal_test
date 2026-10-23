#ifndef STRUCT_TYPEDEF_H
#define STRUCT_TYPEDEF_H

#include <stdint.h>

typedef unsigned char bool_t;
typedef float  fp32;
typedef double fp64;

#ifndef __packed
#define __packed __attribute__((packed))
#endif

#endif /* STRUCT_TYPEDEF_H */
