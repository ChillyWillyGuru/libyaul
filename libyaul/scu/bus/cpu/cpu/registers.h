/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _REGISTERS_H_
#define _REGISTERS_H_

#include <inttypes.h>

struct cpu_registers {
        uint32_t sp;
        uint32_t r[15]; /* Registers r0..r14 */
        uint32_t macl;
        uint32_t mach;
        uint32_t vbr;
        uint32_t gbr;
        uint32_t pr;
        uint32_t pc;
        uint32_t sr;
} __attribute__ ((packed));

#endif /* !_REGISTERS_H_ */