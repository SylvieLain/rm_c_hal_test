#ifndef STRUCT_TYPEDEF_H
#define STRUCT_TYPEDEF_H

#include <stdint.h>

typedef unsigned char bool_t;
typedef float  fp32;
typedef double fp64;

typedef struct
{
  uint8_t  *buf;
  uint16_t  size;              // 2 的幂
  volatile uint16_t head;      // 生产者写
  volatile uint16_t tail;      // 消费者写
} ring_t;

#ifndef __packed
#define __packed __attribute__((packed))
#endif

#endif /* STRUCT_TYPEDEF_H */
