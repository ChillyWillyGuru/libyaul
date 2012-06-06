/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <inttypes.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#include "cons.h"

#define CONS_ATTRIBUTE_RESET_ALL_ATTRIBUTES 0
#define CONS_ATTRIBUTE_BRIGHT   1
#define CONS_ATTRIBUTE_DIM      2
#define CONS_ATTRIBUTE_UNDERSCORE 4
#define CONS_ATTRIBUTE_BLINK    5
#define CONS_ATTRIBUTE_REVERSE  7
#define CONS_ATTRIBUTE_HIDDEN   8

#define CONS_PALETTE_FG_BLACK   30
#define CONS_PALETTE_FG_RED     31
#define CONS_PALETTE_FG_GREEN   32
#define CONS_PALETTE_FG_YELLOW  33
#define CONS_PALETTE_FG_BLUE    34
#define CONS_PALETTE_FG_MAGENTA 35
#define CONS_PALETTE_FG_CYAN    36
#define CONS_PALETTE_FG_WHITE   37
#define CONS_PALETTE_FG_NORMALIZE(c) ((c) - CONS_PALETTE_FG_BLACK)

#define CONS_PALETTE_BG_BLACK   40
#define CONS_PALETTE_BG_RED     41
#define CONS_PALETTE_BG_GREEN   42
#define CONS_PALETTE_BG_YELLOW  43
#define CONS_PALETTE_BG_BLUE    44
#define CONS_PALETTE_BG_MAGENTA 45
#define CONS_PALETTE_BG_CYAN    46
#define CONS_PALETTE_BG_WHITE   47
#define CONS_PALETTE_BG_NORMALIZE(c) ((c) - CONS_PALETTE_BG_BLACK)

static void print_character(struct cons *, int);
static void print_escape_character(struct cons *, int);
static void print_csi_dispatch(struct cons *, int, int *, int);

static bool cursor_column_exceeded(struct cons *, int16_t);
static bool cursor_row_exceeded(struct cons *, uint32_t);
static void cursor_column_advance(struct cons *, int16_t);
static void cursor_column_set(struct cons *, int16_t);
static void cursor_row_advance(struct cons *, uint16_t);
static void cursor_row_set(struct cons *, int16_t);

static void vt_parser_callback(vt_parse_t *, vt_parse_action_t, int);

static uint8_t fg = CONS_PALETTE_FG_WHITE;
static uint8_t bg = CONS_PALETTE_BG_BLACK;
static uint8_t attribute = CONS_ATTRIBUTE_RESET_ALL_ATTRIBUTES;

void
cons_write(struct cons *cons, const char *s)
{
        size_t slen;

        if ((slen = strlen(s)) == 0)
                return;

        vt_parse(&cons->vt_parser, s, slen);
}

void
cons_reset(struct cons *cons)
{

        vt_parse_init(&cons->vt_parser, vt_parser_callback, cons);

        cons->reset(cons);

        cons->cursor.col = 0;
        cons->cursor.row = 0;
}

static void
vt_parser_callback(vt_parse_t *parser, vt_parse_action_t action, int ch)
{
        struct cons *cons;

        cons = (struct cons *)parser->user_data;
        switch (action) {
        case VT_PARSE_ACTION_PRINT:
                print_character(cons, ch);
                break;
        case VT_PARSE_ACTION_EXECUTE:
                print_escape_character(cons, ch);
                break;
        case VT_PARSE_ACTION_CSI_DISPATCH:
                print_csi_dispatch(cons, ch, &parser->params[0],
                    parser->num_params);
                break;
        default:
                break;
        }
}

/*
 * Return true if the current cursor position plus an X amount of
 * columns is out of bounds.
 */
static bool __attribute__ ((unused))
cursor_column_exceeded(struct cons *cons, int16_t x)
{
        int16_t col;

        col = cons->cursor.col + x;
        return (col < 0) || (col >= COLS);
}

/*
 * Return true if the current cursor position plus an Y amount of rows
 * is out of bounds.
 */
static bool __attribute__ ((unused))
cursor_row_exceeded(struct cons *cons, uint32_t y)
{

        return (cons->cursor.col + y) >= ROWS;
}

/*
 * Advance the cursor an X amount of rows.
 */
static void __attribute__ ((unused))
cursor_row_advance(struct cons *cons, uint16_t y)
{

        cons->cursor.row += y;
}

/*
 * Set the cursor an X amount of rows.
 */
static void __attribute__ ((unused))
cursor_row_set(struct cons *cons, int16_t y)
{
        cons->cursor.row = y;
}

/*
 * Advance the cursor an X amount of columns.
 */
static void __attribute__ ((unused))
cursor_column_advance(struct cons *cons, int16_t x)
{

        cons->cursor.col += x;
}

/*
 * Set the cursor an X amount of columns iff it does not exceed COLS.
 */
static void __attribute__ ((unused))
cursor_column_set(struct cons *cons, int16_t x)
{

        cons->cursor.col = x;
}

static void
print_character(struct cons *cons, int ch)
{

        if (cursor_column_exceeded(cons, 0)) {
                cursor_column_set(cons, 0);
                cursor_row_advance(cons, 1);
        }

        cons->write(cons, (char)ch,
            CONS_PALETTE_FG_NORMALIZE(fg),
            CONS_PALETTE_BG_NORMALIZE(bg));
        cursor_column_advance(cons, 1);
}

static void
print_escape_character(struct cons *cons, int ch)
{
        int16_t tab;

        switch (ch) {
        case '\0':
                break;
        case '\a':
                break;
        case '\b':
                if (!cursor_column_exceeded(cons, -1))
                        cursor_column_advance(cons, -1);
                break;
        case '\f':
                break;
        case '\n':
                cursor_column_set(cons, 0);
                cursor_row_advance(cons, 1);
                break;
        case '\r':
                cursor_column_set(cons, 0);
                break;
        case '\t':
                tab = TAB_WIDTH;

                if (cursor_column_exceeded(cons, TAB_WIDTH)) {
                        if ((tab = (COLS - cons->cursor.col - 1)) < 0)
                                break;
                }

                cursor_column_advance(cons, tab);
                break;
        case '\v':
                cursor_row_advance(cons, 1);
                break;
        }
}

static void
print_csi_dispatch(struct cons *cons, int ch, int *params, int num_params)
{
        int16_t col;
        int16_t row;

        uint16_t ofs;

        switch (ch) {
        case 'A':
                /* ESC [ Pn A */

                /* A parameter value of zero or one moves the active
                 * position one line upward */
                if (num_params == 0)
                        break;

                row = (params[0] == 0) ? 1 : params[0];
                if (cursor_row_exceeded(cons, -row))
                        row = cons->cursor.row;
                cursor_row_advance(cons, -row);
                break;
        case 'B':
                /* ESC [ Pn B */

                /* A parameter value of zero or one moves the active
                 * position one line downward */
                if (num_params == 0)
                        break;

                row = (params[0] == 0) ? 1 : params[0];
                if (cursor_row_exceeded(cons, row))
                        row = ROWS - cons->cursor.row - 1;
                cursor_row_advance(cons, row);
                break;
        case 'C':
                /* ESC [ Pn C */

                /* A parameter value of zero or one moves the active
                 * position one position to the right. A parameter value
                 * of n moves the active position n positions to the
                 * right */

                if (num_params == 0)
                        break;

                col = (params[0] == 0) ? 1 : params[0];
                if (cursor_column_exceeded(cons, col))
                        col = COLS - cons->cursor.col - 1;
                cursor_column_advance(cons, col);
                break;
        case 'D':
                /* ESC [ Pn D */

                /* A parameter value of zero or one moves the active
                 * position one position to the left. A parameter value
                 * of n moves the active position n positions to the
                 * left */

                if (num_params == 0)
                        break;

                col = (params[0] == 0) ? 1 : params[0];
                if (cursor_column_exceeded(cons, -col))
                        col = cons->cursor.col;
                cursor_column_advance(cons, -col);
                break;
        case 'H':
                /* ESC [ Pn ; Pn H */

                /* This sequence has two parameter values, the first
                 * specifying the line position and the second
                 * specifying the column position */
                if (((num_params & 1) != 0) && (num_params > 2))
                        break;

                col = params[1];
                row = params[0];

                if (num_params == 0) {
                        col = 0;
                        row = 0;
                }

                if (cursor_column_exceeded(cons, col))
                        col = COLS - 1;

                if (cursor_row_exceeded(cons, row))
                        row = ROWS - 1;

                cursor_column_set(cons, col);
                cursor_row_set(cons, row);
                break;
        case 'J':
                /* ESC [ Ps J */
                if (num_params == 0)
                        break;

                switch (params[0]) {
                case 0:
                        /* Erase from the active position to the end of
                         * the screen, inclusive (default) */
                        break;
                case 1:
                        /* Erase from start of the screen to the active
                         * position, inclusive */
                        break;
                case 2:
                        /* Erase all of the display –- all lines are
                         * erased, changed to single-width, and the
                         * cursor does not move. */
                        break;
                default:
                        break;
                }
                break;
        case 'K':
                /* ESC [ Ps K */
                if (num_params == 0)
                        break;

                switch (params[0]) {
                case 0:
                        /* Erase from the active position to the end of
                         * the line, inclusive (default) */
                        break;
                case 1:
                        /* Erase from the start of the screen to the
                         * active position, inclusive */
                        break;
                case 2:
                        /* Erase all of the line, inclusive */
                        break;
                default:
                        break;
                }
                break;
        case 'm':
                /* ESC [ Ps ; . . . ; Ps m */

                if ((num_params & 1) != 0)
                        /* Number of parameters is odd */
                        break;

                if (num_params == 0) {
                        fg = CONS_PALETTE_FG_WHITE;
                        bg = CONS_PALETTE_BG_BLACK;
                        break;
                }

                for (ofs = 0; ofs < num_params; ofs += 2) {
                        /* Attribute */
                        switch (params[ofs]) {
                        case CONS_ATTRIBUTE_RESET_ALL_ATTRIBUTES:
                                fg = CONS_PALETTE_FG_WHITE;
                                bg = CONS_PALETTE_BG_BLACK;
                                return;
                        case CONS_ATTRIBUTE_BRIGHT:
                                attribute = 8;
                                break;
                        case CONS_ATTRIBUTE_DIM:
                                return;
                        case CONS_ATTRIBUTE_UNDERSCORE:
                                return;
                        case CONS_ATTRIBUTE_BLINK:
                                return;
                        case CONS_ATTRIBUTE_REVERSE:
                                return;
                        case CONS_ATTRIBUTE_HIDDEN:
                                return;
                        default:
                                attribute = CONS_ATTRIBUTE_RESET_ALL_ATTRIBUTES;
                                break;
                        }

                        /* Foreground & background */
                        switch (params[ofs + 1]) {
                        case CONS_PALETTE_FG_BLACK:
                        case CONS_PALETTE_FG_RED:
                        case CONS_PALETTE_FG_GREEN:
                        case CONS_PALETTE_FG_YELLOW:
                        case CONS_PALETTE_FG_BLUE:
                        case CONS_PALETTE_FG_MAGENTA:
                        case CONS_PALETTE_FG_CYAN:
                        case CONS_PALETTE_FG_WHITE:
                                fg = params[ofs + 1] + attribute;
                                break;
                        case CONS_PALETTE_BG_BLACK:
                        case CONS_PALETTE_BG_RED:
                        case CONS_PALETTE_BG_GREEN:
                        case CONS_PALETTE_BG_YELLOW:
                        case CONS_PALETTE_BG_BLUE:
                        case CONS_PALETTE_BG_MAGENTA:
                        case CONS_PALETTE_BG_CYAN:
                        case CONS_PALETTE_BG_WHITE:
                                bg = params[ofs + 1] + attribute;
                                break;
                        }
                        break;
                }
        }
}
