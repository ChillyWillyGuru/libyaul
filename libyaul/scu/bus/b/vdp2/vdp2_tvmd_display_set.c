/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <vdp2/tvmd.h>

#include "vdp2-internal.h"

void
vdp2_tvmd_display_set(void)
{
        uint16_t tvmd;

        tvmd = MEMORY_READ(16, VDP2(TVMD));
        tvmd |= 0x8000;

        MEMORY_WRITE(16, VDP2(TVMD), tvmd);
}
