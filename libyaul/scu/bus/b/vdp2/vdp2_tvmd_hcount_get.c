/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <vdp2/tvmd.h>

#include "vdp2-internal.h"

uint16_t
vdp2_tvmd_hcount_get(void)
{

        for (; ((MEMORY_READ(16, VDP2(EXTEN)) & 0x0200) == 0x0200); );

        return MEMORY_READ(16, VDP2(HCNT));
}
