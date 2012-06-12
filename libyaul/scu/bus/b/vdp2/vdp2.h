/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _VDP2_H_
#define _VDP2_H_

#include <vdp2/cram.h>
#include <vdp2/pn.h>
#include <vdp2/priority.h>
#include <vdp2/scrn.h>
#include <vdp2/tvmd.h>
#include <vdp2/vram.h>

#include <irq-mux.h>

extern void vdp2_init(void);

extern irq_mux_t vdp2_vblank_in_irq_mux;
extern irq_mux_t vdp2_vblank_out_irq_mux;

#endif /* !_VDP2_H_ */
