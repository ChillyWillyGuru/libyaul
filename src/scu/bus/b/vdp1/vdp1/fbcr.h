/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _FBCR_H_
#define _FBCR_H_

enum fbcr_interlace_type {
        FBCR_DID = 0x00,        /* Double Interlace Disable */
        FBCR_DIL = 0x04,        /* Double Interlace Line */
        FBCR_DIE = 0x08         /* Double Interlace Enable */
};

enum fbcr_erase_mode_type {
        FBCR_VBD = 0x00,        /* V-BLANK Erase Write Disable */
        FBCR_VBE = 0x08         /* V-BLANK Erase Write Enable */
};

extern void vdp1_fbcr_bpp_set(unsigned char);
extern void vdp1_fbcr_change_set(void);
extern void vdp1_fbcr_erase_coordinates_set(unsigned short, unsigned short, unsigned short, unsigned short, unsigned short);
extern void vdp1_fbcr_erase_mode_set(enum fbcr_erase_mode_type);
extern void vdp1_fbcr_erase_set();
extern void vdp1_fbcr_interlace_set(enum fbcr_interlace_type);
extern void vdp1_fbcr_rotate_set(void);

#endif
