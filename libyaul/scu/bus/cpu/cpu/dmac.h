/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _DMAC_H_
#define _DMAC_H_

#include <inttypes.h>
#include <stddef.h>

#define CPU_DMAC_CHANNEL(x)     ((x))

struct cpu_channel_cfg {
        uint32_t ch;

        struct {
#define CPU_DMAC_DESTINATION_FIXED 0x00000000
#define CPU_DMAC_DESTINATION_INCREMENT 0x00004000
#define CPU_DMAC_DESTINATION_DECREMENT 0x00008000
                uint32_t mode;
                void *ptr;
        } dst;

        struct {
#define CPU_DMAC_SOURCE_FIXED 0x00000000
#define CPU_DMAC_SOURCE_INCREMENT 0x00001000
#define CPU_DMAC_SOURCE_DECREMENT 0x00002000
                uint32_t mode;
                const void *ptr;
        } src;

        size_t len;
        uint32_t xfer_size;

        uint8_t vector;
        void (*ihr)(void);
};

extern void cpu_dmac_channel_set(struct cpu_channel_cfg *);
extern void cpu_dmac_channel_start(uint8_t);

#endif /* !_DMAC_H */
