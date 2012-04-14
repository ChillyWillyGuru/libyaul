/*
 * Copyright (c) 2008-2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <inttypes.h>

#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#include <vdp2/cram.h>
#include <vdp2/vram.h>
#include <vdp2/pn.h>
#include <vdp2.h>

#include <monitor.h>

#define FONT_H          8
#define FONT_W          8

/* Half the normal size, 80x25 should be used */
#define ROWS            28
#define COLS            40
#define TAB_WIDTH       2

struct {
        uint16_t *pnt[4];
        uint32_t *character;

        uint16_t col; /* Current column position in tile memory */
        uint16_t row; /* Current row position in tile memory */

        uint32_t character_no;
        uint16_t x; /* Current column position in map memory */
        uint16_t y; /* Current row position in map memory */
} info;

static const uint32_t font[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7E, 0x81, 0xA5, 0x81, 0xBD, 0x99, 0x81, 0x7E,
        0x7E, 0xFF, 0xDB, 0xFF, 0xC3, 0xE7, 0xFF, 0x7E, 0x36, 0x7F, 0x7F, 0x7F, 0x3E, 0x1C, 0x08, 0x00,
        0x08, 0x1C, 0x3E, 0x7F, 0x3E, 0x1C, 0x08, 0x00, 0x1C, 0x3E, 0x1C, 0x7F, 0x7F, 0x6B, 0x08, 0x1C,
        0x08, 0x08, 0x1C, 0x3E, 0x7F, 0x3E, 0x08, 0x1C, 0x00, 0x00, 0x18, 0x3C, 0x3C, 0x18, 0x00, 0x00,
        0xFF, 0xFF, 0xE7, 0xC3, 0xC3, 0xE7, 0xFF, 0xFF, 0x00, 0x3C, 0x66, 0x42, 0x42, 0x66, 0x3C, 0x00,
        0xFF, 0xC3, 0x99, 0xBD, 0xBD, 0x99, 0xC3, 0xFF, 0xF0, 0xE0, 0xF0, 0xBE, 0x33, 0x33, 0x33, 0x1E,
        0x3C, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x7E, 0x18, 0xFC, 0xCC, 0xFC, 0x0C, 0x0C, 0x0E, 0x0F, 0x07,
        0xFE, 0xC6, 0xFE, 0xC6, 0xC6, 0xE6, 0x67, 0x03, 0x99, 0x5A, 0x3C, 0xE7, 0xE7, 0x3C, 0x5A, 0x99,
        0x01, 0x07, 0x1F, 0x7F, 0x1F, 0x07, 0x01, 0x00, 0x40, 0x70, 0x7C, 0x7F, 0x7C, 0x70, 0x40, 0x00,
        0x18, 0x3C, 0x7E, 0x18, 0x18, 0x7E, 0x3C, 0x18, 0x66, 0x66, 0x66, 0x66, 0x66, 0x00, 0x66, 0x00,
        0xFE, 0xDB, 0xDB, 0xDE, 0xD8, 0xD8, 0xD8, 0x00, 0x7E, 0xC3, 0x1E, 0x33, 0x33, 0x1E, 0x31, 0x1F,
        0x00, 0x00, 0x00, 0x00, 0x7E, 0x7E, 0x7E, 0x00, 0x18, 0x3C, 0x7E, 0x18, 0x7E, 0x3C, 0x18, 0xFF,
        0x18, 0x3C, 0x7E, 0x18, 0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x3C, 0x18, 0x00,
        0x00, 0x18, 0x30, 0x7F, 0x30, 0x18, 0x00, 0x00, 0x00, 0x0C, 0x06, 0x7F, 0x06, 0x0C, 0x00, 0x00,
        0x00, 0x00, 0x03, 0x03, 0x03, 0x7F, 0x00, 0x00, 0x00, 0x24, 0x66, 0xFF, 0x66, 0x24, 0x00, 0x00,
        0x00, 0x18, 0x3C, 0x7E, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x7E, 0x3C, 0x18, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x1E, 0x1E, 0x0C, 0x0C, 0x00, 0x0C, 0x00,
        0x36, 0x36, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x36, 0x7F, 0x36, 0x7F, 0x36, 0x36, 0x00,
        0x0C, 0x3E, 0x03, 0x1E, 0x30, 0x1F, 0x0C, 0x00, 0x00, 0x63, 0x33, 0x18, 0x0C, 0x66, 0x63, 0x00,
        0x1C, 0x36, 0x1C, 0x6E, 0x3B, 0x33, 0x6E, 0x00, 0x06, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x18, 0x0C, 0x06, 0x06, 0x06, 0x0C, 0x18, 0x00, 0x06, 0x0C, 0x18, 0x18, 0x18, 0x0C, 0x06, 0x00,
        0x00, 0x66, 0x3C, 0xFF, 0x3C, 0x66, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x3F, 0x0C, 0x0C, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x0C, 0x06, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x00, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x03, 0x01, 0x00,
        0x1E, 0x33, 0x3B, 0x3F, 0x37, 0x33, 0x1E, 0x00, 0x0C, 0x0F, 0x0C, 0x0C, 0x0C, 0x0C, 0x3F, 0x00,
        0x1E, 0x33, 0x30, 0x1C, 0x06, 0x33, 0x3F, 0x00, 0x1E, 0x33, 0x30, 0x1C, 0x30, 0x33, 0x1E, 0x00,
        0x38, 0x3C, 0x36, 0x33, 0x7F, 0x30, 0x30, 0x00, 0x3F, 0x03, 0x1F, 0x30, 0x30, 0x33, 0x1E, 0x00,
        0x1C, 0x06, 0x03, 0x1F, 0x33, 0x33, 0x1E, 0x00, 0x3F, 0x33, 0x30, 0x18, 0x0C, 0x06, 0x06, 0x00,
        0x1E, 0x33, 0x33, 0x1E, 0x33, 0x33, 0x1E, 0x00, 0x1E, 0x33, 0x33, 0x3E, 0x30, 0x18, 0x0E, 0x00,
        0x00, 0x00, 0x0C, 0x0C, 0x00, 0x0C, 0x0C, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x00, 0x0E, 0x0C, 0x06,
        0x18, 0x0C, 0x06, 0x03, 0x06, 0x0C, 0x18, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x3F, 0x00, 0x00, 0x00,
        0x06, 0x0C, 0x18, 0x30, 0x18, 0x0C, 0x06, 0x00, 0x1E, 0x33, 0x30, 0x18, 0x0C, 0x00, 0x0C, 0x00,
        0x3E, 0x63, 0x7B, 0x7B, 0x7B, 0x03, 0x1E, 0x00, 0x0C, 0x1E, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x00,
        0x3F, 0x66, 0x66, 0x3E, 0x66, 0x66, 0x3F, 0x00, 0x3C, 0x66, 0x03, 0x03, 0x03, 0x66, 0x3C, 0x00,
        0x3F, 0x36, 0x66, 0x66, 0x66, 0x36, 0x3F, 0x00, 0x7F, 0x46, 0x16, 0x1E, 0x16, 0x46, 0x7F, 0x00,
        0x7F, 0x46, 0x16, 0x1E, 0x16, 0x06, 0x0F, 0x00, 0x3C, 0x66, 0x03, 0x03, 0x73, 0x66, 0x7C, 0x00,
        0x33, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x33, 0x00, 0x1E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00,
        0x78, 0x30, 0x30, 0x30, 0x33, 0x33, 0x1E, 0x00, 0x67, 0x66, 0x36, 0x1E, 0x36, 0x66, 0x67, 0x00,
        0x0F, 0x06, 0x06, 0x06, 0x46, 0x66, 0x7F, 0x00, 0x63, 0x77, 0x7F, 0x6B, 0x63, 0x63, 0x63, 0x00,
        0x63, 0x67, 0x6F, 0x7B, 0x73, 0x63, 0x63, 0x00, 0x1C, 0x36, 0x63, 0x63, 0x63, 0x36, 0x1C, 0x00,
        0x3F, 0x66, 0x66, 0x3E, 0x06, 0x06, 0x0F, 0x00, 0x1E, 0x33, 0x33, 0x33, 0x3B, 0x1E, 0x38, 0x00,
        0x3F, 0x66, 0x66, 0x3E, 0x1E, 0x36, 0x67, 0x00, 0x1E, 0x33, 0x07, 0x1C, 0x38, 0x33, 0x1E, 0x00,
        0x3F, 0x2D, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x3F, 0x00,
        0x33, 0x33, 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x00, 0x63, 0x63, 0x63, 0x6B, 0x7F, 0x77, 0x63, 0x00,
        0x63, 0x63, 0x36, 0x1C, 0x36, 0x63, 0x63, 0x00, 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x0C, 0x1E, 0x00,
        0x7F, 0x33, 0x19, 0x0C, 0x46, 0x63, 0x7F, 0x00, 0x1E, 0x06, 0x06, 0x06, 0x06, 0x06, 0x1E, 0x00,
        0x03, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x40, 0x00, 0x1E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1E, 0x00,
        0x08, 0x1C, 0x36, 0x63, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
        0x0C, 0x0C, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1E, 0x30, 0x3E, 0x33, 0x6E, 0x00,
        0x07, 0x06, 0x3E, 0x66, 0x66, 0x66, 0x3D, 0x00, 0x00, 0x00, 0x1E, 0x33, 0x03, 0x33, 0x1E, 0x00,
        0x38, 0x30, 0x30, 0x3E, 0x33, 0x33, 0x6E, 0x00, 0x00, 0x00, 0x1E, 0x33, 0x3F, 0x03, 0x1E, 0x00,
        0x1C, 0x36, 0x06, 0x0F, 0x06, 0x06, 0x0F, 0x00, 0x00, 0x00, 0x6E, 0x33, 0x33, 0x3E, 0x30, 0x1F,
        0x07, 0x06, 0x36, 0x6E, 0x66, 0x66, 0x67, 0x00, 0x0C, 0x00, 0x0E, 0x0C, 0x0C, 0x0C, 0x1E, 0x00,
        0x18, 0x00, 0x1E, 0x18, 0x18, 0x18, 0x1B, 0x0E, 0x07, 0x06, 0x66, 0x36, 0x1E, 0x36, 0x67, 0x00,
        0x0E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00, 0x00, 0x00, 0x37, 0x7F, 0x6B, 0x63, 0x63, 0x00,
        0x00, 0x00, 0x1F, 0x33, 0x33, 0x33, 0x33, 0x00, 0x00, 0x00, 0x1E, 0x33, 0x33, 0x33, 0x1E, 0x00,
        0x00, 0x00, 0x3B, 0x66, 0x66, 0x3E, 0x06, 0x0F, 0x00, 0x00, 0x6E, 0x33, 0x33, 0x3E, 0x30, 0x78,
        0x00, 0x00, 0x1B, 0x36, 0x36, 0x06, 0x0F, 0x00, 0x00, 0x00, 0x3E, 0x03, 0x1E, 0x30, 0x1F, 0x00,
        0x08, 0x0C, 0x3E, 0x0C, 0x0C, 0x2C, 0x18, 0x00, 0x00, 0x00, 0x33, 0x33, 0x33, 0x33, 0x6E, 0x00,
        0x00, 0x00, 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x00, 0x00, 0x00, 0x63, 0x63, 0x6B, 0x7F, 0x36, 0x00,
        0x00, 0x00, 0x63, 0x36, 0x1C, 0x36, 0x63, 0x00, 0x00, 0x00, 0x33, 0x33, 0x33, 0x3E, 0x30, 0x1F,
        0x00, 0x00, 0x3F, 0x19, 0x0C, 0x26, 0x3F, 0x00, 0x38, 0x0C, 0x0C, 0x07, 0x0C, 0x0C, 0x38, 0x00,
        0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x00, 0x07, 0x0C, 0x0C, 0x38, 0x0C, 0x0C, 0x07, 0x00,
        0x6E, 0x3B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x1C, 0x36, 0x63, 0x63, 0x63, 0x7F, 0x00,
        0x1E, 0x33, 0x03, 0x33, 0x1E, 0x18, 0x30, 0x1E, 0x00, 0x33, 0x00, 0x33, 0x33, 0x33, 0x7E, 0x00,
        0x38, 0x00, 0x1E, 0x33, 0x3F, 0x03, 0x1E, 0x00, 0x7E, 0xC3, 0x3C, 0x60, 0x7C, 0x66, 0xFC, 0x00,
        0x33, 0x00, 0x1E, 0x30, 0x3E, 0x33, 0x7E, 0x00, 0x07, 0x00, 0x1E, 0x30, 0x3E, 0x33, 0x7E, 0x00,
        0x0C, 0x0C, 0x1E, 0x30, 0x3E, 0x33, 0x7E, 0x00, 0x00, 0x00, 0x3E, 0x03, 0x03, 0x3E, 0x60, 0x3C,
        0x7E, 0xC3, 0x3C, 0x66, 0x7E, 0x06, 0x3C, 0x00, 0x33, 0x00, 0x1E, 0x33, 0x3F, 0x03, 0x1E, 0x00,
        0x07, 0x00, 0x1E, 0x33, 0x3F, 0x03, 0x1E, 0x00, 0x33, 0x00, 0x0E, 0x0C, 0x0C, 0x0C, 0x1E, 0x00,
        0x3E, 0x63, 0x1C, 0x18, 0x18, 0x18, 0x3C, 0x00, 0x07, 0x00, 0x0E, 0x0C, 0x0C, 0x0C, 0x1E, 0x00,
        0x33, 0x0C, 0x1E, 0x33, 0x33, 0x3F, 0x33, 0x00, 0x0C, 0x0C, 0x00, 0x1E, 0x33, 0x3F, 0x33, 0x00,
        0x38, 0x00, 0x3F, 0x06, 0x1E, 0x06, 0x3F, 0x00, 0x00, 0x00, 0xFE, 0x30, 0xFE, 0x33, 0xFE, 0x00,
        0x7C, 0x36, 0x33, 0x7F, 0x33, 0x33, 0x73, 0x00, 0x1E, 0x33, 0x00, 0x1E, 0x33, 0x33, 0x1E, 0x00,
        0x00, 0x33, 0x00, 0x1E, 0x33, 0x33, 0x1E, 0x00, 0x00, 0x07, 0x00, 0x1E, 0x33, 0x33, 0x1E, 0x00,
        0x1E, 0x33, 0x00, 0x33, 0x33, 0x33, 0x7E, 0x00, 0x00, 0x07, 0x00, 0x33, 0x33, 0x33, 0x7E, 0x00,
        0x00, 0x33, 0x00, 0x33, 0x33, 0x3F, 0x30, 0x1F, 0x63, 0x1C, 0x3E, 0x63, 0x63, 0x3E, 0x1C, 0x00,
        0x33, 0x00, 0x33, 0x33, 0x33, 0x33, 0x1E, 0x00, 0x18, 0x18, 0x7E, 0x03, 0x03, 0x7E, 0x18, 0x18,
        0x1C, 0x36, 0x26, 0x0F, 0x06, 0x67, 0x3F, 0x00, 0x33, 0x33, 0x1E, 0x3F, 0x0C, 0x3F, 0x0C, 0x00,
        0x0F, 0x1B, 0x1B, 0x2F, 0x33, 0x7B, 0x33, 0x70, 0x70, 0xD8, 0x18, 0x7E, 0x18, 0x18, 0x1B, 0x0E,
        0x38, 0x00, 0x1E, 0x30, 0x3E, 0x33, 0x7E, 0x00, 0x1C, 0x00, 0x0E, 0x0C, 0x0C, 0x0C, 0x1E, 0x00,
        0x00, 0x38, 0x00, 0x1E, 0x33, 0x33, 0x1E, 0x00, 0x00, 0x38, 0x00, 0x33, 0x33, 0x33, 0x7E, 0x00,
        0x00, 0x1F, 0x00, 0x1F, 0x33, 0x33, 0x33, 0x00, 0x3F, 0x00, 0x33, 0x37, 0x3F, 0x3B, 0x33, 0x00,
        0x3C, 0x36, 0x36, 0x7C, 0x00, 0x7E, 0x00, 0x00, 0x3C, 0x66, 0x66, 0x3C, 0x00, 0x7E, 0x00, 0x00,
        0x0C, 0x00, 0x0C, 0x06, 0x03, 0x33, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x03, 0x03, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x3F, 0x30, 0x30, 0x00, 0x00, 0x63, 0x33, 0x1B, 0x7C, 0xC6, 0x73, 0x19, 0xF8,
        0x63, 0x33, 0x1B, 0xCF, 0xE6, 0xF3, 0xF9, 0xC0, 0x00, 0x18, 0x00, 0x18, 0x18, 0x3C, 0x3C, 0x18,
        0x00, 0xCC, 0x66, 0x33, 0x66, 0xCC, 0x00, 0x00, 0x00, 0x33, 0x66, 0xCC, 0x66, 0x33, 0x00, 0x00,
        0x44, 0x11, 0x44, 0x11, 0x44, 0x11, 0x44, 0x11, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
        0xBB, 0xEE, 0xBB, 0xEE, 0xBB, 0xEE, 0xBB, 0xEE, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
        0x18, 0x18, 0x18, 0x18, 0x1F, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1F, 0x18, 0x1F, 0x18, 0x18, 0x18,
        0x6C, 0x6C, 0x6C, 0x6C, 0x6F, 0x6C, 0x6C, 0x6C, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x6C, 0x6C, 0x6C,
        0x00, 0x00, 0x1F, 0x18, 0x1F, 0x18, 0x18, 0x18, 0x6C, 0x6C, 0x6F, 0x60, 0x6F, 0x6C, 0x6C, 0x6C,
        0x6C, 0x6C, 0x6C, 0x6C, 0x6C, 0x6C, 0x6C, 0x6C, 0x00, 0x00, 0x7F, 0x60, 0x6F, 0x6C, 0x6C, 0x6C,
        0x6C, 0x6C, 0x6F, 0x60, 0x7F, 0x00, 0x00, 0x00, 0x6C, 0x6C, 0x6C, 0x6C, 0x7F, 0x00, 0x00, 0x00,
        0x18, 0x18, 0x1F, 0x18, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x18, 0x18, 0x18,
        0x18, 0x18, 0x18, 0x18, 0xF8, 0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0xFF, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xFF, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0xF8, 0x18, 0x18, 0x18,
        0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0xFF, 0x18, 0x18, 0x18,
        0x18, 0x18, 0xF8, 0x18, 0xF8, 0x18, 0x18, 0x18, 0x6C, 0x6C, 0x6C, 0x6C, 0xEC, 0x6C, 0x6C, 0x6C,
        0x6C, 0x6C, 0xEC, 0x0C, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0x0C, 0xEC, 0x6C, 0x6C, 0x6C,
        0x6C, 0x6C, 0xEF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xEF, 0x6C, 0x6C, 0x6C,
        0x6C, 0x6C, 0xEC, 0x0C, 0xEC, 0x6C, 0x6C, 0x6C, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00,
        0x6C, 0x6C, 0xEF, 0x00, 0xEF, 0x6C, 0x6C, 0x6C, 0x18, 0x18, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00,
        0x6C, 0x6C, 0x6C, 0x6C, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x18, 0x18, 0x18,
        0x00, 0x00, 0x00, 0x00, 0xFF, 0x6C, 0x6C, 0x6C, 0x6C, 0x6C, 0x6C, 0x6C, 0xFC, 0x00, 0x00, 0x00,
        0x18, 0x18, 0xF8, 0x18, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x18, 0xF8, 0x18, 0x18, 0x18,
        0x00, 0x00, 0x00, 0x00, 0xFC, 0x6C, 0x6C, 0x6C, 0x6C, 0x6C, 0x6C, 0x6C, 0xEF, 0x6C, 0x6C, 0x6C,
        0x18, 0x18, 0xFF, 0x00, 0xFF, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1F, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xF8, 0x18, 0x18, 0x18, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
        0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x6E, 0x3B, 0x13, 0x3B, 0x6E, 0x00, 0x00, 0x1E, 0x33, 0x1F, 0x33, 0x1F, 0x03, 0x03,
        0x00, 0x7F, 0x63, 0x03, 0x03, 0x03, 0x03, 0x00, 0x00, 0x7F, 0x36, 0x36, 0x36, 0x36, 0x36, 0x00,
        0x7F, 0x66, 0x0C, 0x18, 0x0C, 0x66, 0x7F, 0x00, 0x00, 0x00, 0x7E, 0x33, 0x33, 0x33, 0x1E, 0x00,
        0x00, 0x66, 0x66, 0x66, 0x66, 0x3E, 0x06, 0x03, 0x00, 0x6E, 0x3B, 0x18, 0x18, 0x18, 0x18, 0x00,
        0x3F, 0x0C, 0x1E, 0x33, 0x33, 0x1E, 0x0C, 0x3F, 0x1C, 0x36, 0x63, 0x7F, 0x63, 0x36, 0x1C, 0x00,
        0x1C, 0x36, 0x63, 0x63, 0x36, 0x36, 0x77, 0x00, 0x38, 0x0C, 0x18, 0x3E, 0x33, 0x33, 0x1E, 0x00,
        0x00, 0x00, 0x7E, 0xDB, 0xDB, 0x7E, 0x00, 0x00, 0x60, 0x30, 0x7E, 0xDB, 0xDB, 0x7E, 0x06, 0x03,
        0x3C, 0x06, 0x03, 0x3F, 0x03, 0x06, 0x3C, 0x00, 0x1E, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x00,
        0x00, 0x3F, 0x00, 0x3F, 0x00, 0x3F, 0x00, 0x00, 0x0C, 0x0C, 0x3F, 0x0C, 0x0C, 0x00, 0x3F, 0x00,
        0x06, 0x0C, 0x18, 0x0C, 0x06, 0x00, 0x3F, 0x00, 0x18, 0x0C, 0x06, 0x0C, 0x18, 0x00, 0x3F, 0x00,
        0x70, 0xD8, 0xD8, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1B, 0x1B, 0x0E,
        0x0C, 0x0C, 0x00, 0x3F, 0x00, 0x0C, 0x0C, 0x00, 0x00, 0x4E, 0x39, 0x00, 0x4E, 0x39, 0x00, 0x00,
        0x1C, 0x36, 0x36, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0xF0, 0x30, 0x30, 0x30, 0x37, 0x36, 0x3C, 0x38,
        0x1E, 0x36, 0x36, 0x36, 0x36, 0x00, 0x00, 0x00, 0x1E, 0x30, 0x1C, 0x06, 0x3E, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x3C, 0x3C, 0x3C, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/*-
 * This LUT holds the pre calculated 256 possibilities of a 4-BPP row
 * can have.
 *
 * By keeping a LUT, we don't have to bit-unpack each byte; instead we
 * can simply look it up and have it bit-unpacked on the fly.
 */

static const uint32_t tile[] = {
        0x00000000, 0xF0000000, 0x0F000000, 0xFF000000,
        0x00F00000, 0xF0F00000, 0x0FF00000, 0xFFF00000,
        0x000F0000, 0xF00F0000, 0x0F0F0000, 0xFF0F0000,
        0x00FF0000, 0xF0FF0000, 0x0FFF0000, 0xFFFF0000,
        0x0000F000, 0xF000F000, 0x0F00F000, 0xFF00F000,
        0x00F0F000, 0xF0F0F000, 0x0FF0F000, 0xFFF0F000,
        0x000FF000, 0xF00FF000, 0x0F0FF000, 0xFF0FF000,
        0x00FFF000, 0xF0FFF000, 0x0FFFF000, 0xFFFFF000,
        0x00000F00, 0xF0000F00, 0x0F000F00, 0xFF000F00,
        0x00F00F00, 0xF0F00F00, 0x0FF00F00, 0xFFF00F00,
        0x000F0F00, 0xF00F0F00, 0x0F0F0F00, 0xFF0F0F00,
        0x00FF0F00, 0xF0FF0F00, 0x0FFF0F00, 0xFFFF0F00,
        0x0000FF00, 0xF000FF00, 0x0F00FF00, 0xFF00FF00,
        0x00F0FF00, 0xF0F0FF00, 0x0FF0FF00, 0xFFF0FF00,
        0x000FFF00, 0xF00FFF00, 0x0F0FFF00, 0xFF0FFF00,
        0x00FFFF00, 0xF0FFFF00, 0x0FFFFF00, 0xFFFFFF00,
        0x000000F0, 0xF00000F0, 0x0F0000F0, 0xFF0000F0,
        0x00F000F0, 0xF0F000F0, 0x0FF000F0, 0xFFF000F0,
        0x000F00F0, 0xF00F00F0, 0x0F0F00F0, 0xFF0F00F0,
        0x00FF00F0, 0xF0FF00F0, 0x0FFF00F0, 0xFFFF00F0,
        0x0000F0F0, 0xF000F0F0, 0x0F00F0F0, 0xFF00F0F0,
        0x00F0F0F0, 0xF0F0F0F0, 0x0FF0F0F0, 0xFFF0F0F0,
        0x000FF0F0, 0xF00FF0F0, 0x0F0FF0F0, 0xFF0FF0F0,
        0x00FFF0F0, 0xF0FFF0F0, 0x0FFFF0F0, 0xFFFFF0F0,
        0x00000FF0, 0xF0000FF0, 0x0F000FF0, 0xFF000FF0,
        0x00F00FF0, 0xF0F00FF0, 0x0FF00FF0, 0xFFF00FF0,
        0x000F0FF0, 0xF00F0FF0, 0x0F0F0FF0, 0xFF0F0FF0,
        0x00FF0FF0, 0xF0FF0FF0, 0x0FFF0FF0, 0xFFFF0FF0,
        0x0000FFF0, 0xF000FFF0, 0x0F00FFF0, 0xFF00FFF0,
        0x00F0FFF0, 0xF0F0FFF0, 0x0FF0FFF0, 0xFFF0FFF0,
        0x000FFFF0, 0xF00FFFF0, 0x0F0FFFF0, 0xFF0FFFF0,
        0x00FFFFF0, 0xF0FFFFF0, 0x0FFFFFF0, 0xFFFFFFF0,
        0x0000000F, 0xF000000F, 0x0F00000F, 0xFF00000F,
        0x00F0000F, 0xF0F0000F, 0x0FF0000F, 0xFFF0000F,
        0x000F000F, 0xF00F000F, 0x0F0F000F, 0xFF0F000F,
        0x00FF000F, 0xF0FF000F, 0x0FFF000F, 0xFFFF000F,
        0x0000F00F, 0xF000F00F, 0x0F00F00F, 0xFF00F00F,
        0x00F0F00F, 0xF0F0F00F, 0x0FF0F00F, 0xFFF0F00F,
        0x000FF00F, 0xF00FF00F, 0x0F0FF00F, 0xFF0FF00F,
        0x00FFF00F, 0xF0FFF00F, 0x0FFFF00F, 0xFFFFF00F,
        0x00000F0F, 0xF0000F0F, 0x0F000F0F, 0xFF000F0F,
        0x00F00F0F, 0xF0F00F0F, 0x0FF00F0F, 0xFFF00F0F,
        0x000F0F0F, 0xF00F0F0F, 0x0F0F0F0F, 0xFF0F0F0F,
        0x00FF0F0F, 0xF0FF0F0F, 0x0FFF0F0F, 0xFFFF0F0F,
        0x0000FF0F, 0xF000FF0F, 0x0F00FF0F, 0xFF00FF0F,
        0x00F0FF0F, 0xF0F0FF0F, 0x0FF0FF0F, 0xFFF0FF0F,
        0x000FFF0F, 0xF00FFF0F, 0x0F0FFF0F, 0xFF0FFF0F,
        0x00FFFF0F, 0xF0FFFF0F, 0x0FFFFF0F, 0xFFFFFF0F,
        0x000000FF, 0xF00000FF, 0x0F0000FF, 0xFF0000FF,
        0x00F000FF, 0xF0F000FF, 0x0FF000FF, 0xFFF000FF,
        0x000F00FF, 0xF00F00FF, 0x0F0F00FF, 0xFF0F00FF,
        0x00FF00FF, 0xF0FF00FF, 0x0FFF00FF, 0xFFFF00FF,
        0x0000F0FF, 0xF000F0FF, 0x0F00F0FF, 0xFF00F0FF,
        0x00F0F0FF, 0xF0F0F0FF, 0x0FF0F0FF, 0xFFF0F0FF,
        0x000FF0FF, 0xF00FF0FF, 0x0F0FF0FF, 0xFF0FF0FF,
        0x00FFF0FF, 0xF0FFF0FF, 0x0FFFF0FF, 0xFFFFF0FF,
        0x00000FFF, 0xF0000FFF, 0x0F000FFF, 0xFF000FFF,
        0x00F00FFF, 0xF0F00FFF, 0x0FF00FFF, 0xFFF00FFF,
        0x000F0FFF, 0xF00F0FFF, 0x0F0F0FFF, 0xFF0F0FFF,
        0x00FF0FFF, 0xF0FF0FFF, 0x0FFF0FFF, 0xFFFF0FFF,
        0x0000FFFF, 0xF000FFFF, 0x0F00FFFF, 0xFF00FFFF,
        0x00F0FFFF, 0xF0F0FFFF, 0x0FF0FFFF, 0xFFF0FFFF,
        0x000FFFFF, 0xF00FFFFF, 0x0F0FFFFF, 0xFF0FFFFF,
        0x00FFFFFF, 0xF0FFFFFF, 0x0FFFFFFF, 0xFFFFFFFF
};

static const uint16_t palette[] = {
        0x0000, 0x800F, 0x81E0, 0x81EF, 0xBC00, 0xBC0F, 0xBDE0, 0xBDEF,
        0x0000, 0x801F, 0x83E0, 0x83FF, 0xFC00, 0xFC1F, 0xFFE0, 0xFFFF
};

static bool bounds(uint32_t);
static uint32_t column(void);
static void advance_row(uint16_t);
static void advance_column(uint16_t);
static void draw(int, struct cha *);
static void newline(void);

void
monitor_init(void)
{
        struct scrn_ch_format cfg;
        uint32_t tmrs[4];
        uint32_t x;
        uint32_t y;

        /* VRAM B1 */
        info.pnt[0] = (uint16_t *)VRAM_BANK_4MBIT(3, 0x18000);
        /* VRAM B1 */
        info.pnt[1] = (uint16_t *)VRAM_BANK_4MBIT(3, 0x18000);
        /* VRAM B1 */
        info.pnt[2] = (uint16_t *)VRAM_BANK_4MBIT(3, 0x18000);
        /* VRAM B1 */
        info.pnt[3] = (uint16_t *)VRAM_BANK_4MBIT(3, 0x18000);
        /* VRAM B1 */
        info.character = (uint32_t *)VRAM_BANK_4MBIT(3, 0x1C000);

        cfg.ch_scrn = SCRN_NBG2;
        cfg.ch_cs = 1 * 1; /* 1x1 cells */
        cfg.ch_pnds = 1; /* 1 word */
        cfg.ch_cnsm = 1; /* Character number supplement mode: 1 */
        cfg.ch_sp = 0;
        cfg.ch_scc = 0;
        cfg.ch_spn = 0;
        cfg.ch_scn = (uint32_t)info.character;
        cfg.ch_pls = 1 * 1; /* 1x1 plane size */
        cfg.ch_map[0] = (uint32_t)info.pnt[0];
        cfg.ch_map[1] = (uint32_t)info.pnt[1];
        cfg.ch_map[2] = (uint32_t)info.pnt[2];
        cfg.ch_map[3] = (uint32_t)info.pnt[3];

        vdp2_scrn_ch_color_set(SCRN_NBG2, SCRN_CHC_16);
        vdp2_scrn_ch_format_set(&cfg);
        vdp2_scrn_priority_set(SCRN_NBG2, 1);

        tmrs[0] = 0xFFFFFFFF;
        tmrs[1] = 0xFFFFFFFF;
        tmrs[2] = 0xFFFFFFFF;
        tmrs[3] = 0xEEEEE662;

        vdp2_vram_cycle_pattern_set(tmrs);

        /* The first character is reserved as whitespace */
        info.character_no = 1;

        /* Wait until we can draw */
        while (vdp2_tvmd_vblank_status_get() == 0);
        while (vdp2_tvmd_vblank_status_get());

        /* Clear the first tile */
        for (y = 0; y < FONT_H; y++)
                info.character[y] = tile[0];

        info.col = 1;
        info.row = 0;

        info.x = 0;
        info.y = 0;

        /* Wait until we can draw */
        while (vdp2_tvmd_vblank_status_get() == 0);
        while (vdp2_tvmd_vblank_status_get());

        /* Clear map */
        for (y = 0; y < ROWS; y++) {
                for (x = 0; x < COLS; x++)
                        info.pnt[0][x + (y << 6)] = PN_CHARACTER_NO((uint32_t)info.character);
        }

        memcpy((uint16_t *)CRAM_BANK(0, 0), palette, sizeof(palette));

        /* Hopefully it won't glitch */
        vdp2_scrn_display_set(SCRN_NBG2);
}

/*
 * The "driver" for the terminal.
 */
void
monitor(int c, struct cha *cha_opt)
{
        int16_t tab;

        switch (c) {
        case '\n': /* New line */
                newline();
                break;
        case '\r': /* Form feed */
                break;
        case '\t': /* Horizontal tab */
                tab = TAB_WIDTH;
                if (bounds(TAB_WIDTH)) {
                        if ((tab = COLS - column() - 1) < 0)
                                break;
                }

                advance_column(tab);
                break;
        case '\v': /* Vertical tab */
                advance_row(1);
                break;
        default:
                if (bounds(0))
                        newline();

                draw(c, cha_opt);
                break;
        }
}

static void
draw(int c, struct cha *cha_opt)
{
        uint32_t c_off;
        uint32_t t_off;
        uint32_t row;
        int y;
        uint32_t fg;
        uint32_t bg;

        uint32_t color_tbl[] = {
                0x00000000, 0x11111111, 0x22222222, 0x33333333,
                0x44444444, 0x55555555, 0x66666666, 0x77777777,
                0x88888888, 0x99999999, 0xAAAAAAAA, 0xBBBBBBBB,
                0xCCCCCCCC, 0xDDDDDDDD, 0xEEEEEEEE, 0xFFFFFFFF
        };

        if (isspace(c) && (cha_opt->bg == BACKGROUND)) {
                info.x++;
                return;
        }

        t_off = c << 3;
        c_off = info.col << 3;

        /* Expand tile. */
        for (y = FONT_H - 1; y >= 0; y--) {
                row = tile[font[y + t_off]];
                fg = color_tbl[cha_opt->fg];
                bg = color_tbl[cha_opt->bg];

                info.character[y + c_off] = (row & fg) | ((row & bg) ^ bg);
        }

        info.pnt[0][info.x + (info.y << 6)] = PN_CHARACTER_NO((uint32_t)info.character) | info.character_no;
        info.character_no++;

        info.col++;
        info.x++;
}

/*
 * Current column position relative to the current line [0..COLS)
 */
static uint32_t
column(void)
{
        return info.x;
}

/*
 * Return true if the current column position plus an X amount of
 * columns is out of bounds.
 */
static bool
bounds(uint32_t x)
{

        return (column() + x) >= COLS;
}

/*
 * Move to a new row and reset the cursor back to the leftmost column.
 */
static void
newline(void)
{

        info.x = 0;
        info.y++;
}

/*
 * Advance an X amount of rows.
 */
static void
advance_row(uint16_t x)
{

        info.y += x;
}

/*
 * Advance an X amount of columns.
 */
static void
advance_column(uint16_t x)
{

        info.x += x;
}
