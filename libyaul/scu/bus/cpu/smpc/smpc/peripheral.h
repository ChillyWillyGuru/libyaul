/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _PERIPHERAL_H_
#define _PERIPHERAL_H_

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>

#include <sys/queue.h>

#include <irq-mux.h>

#define MAX_PORT_DEVICES        6
#define MAX_PORT_DATA_SIZE      255

#define KEY_0                   0x45
#define KEY_1                   0x16
#define KEY_2                   0x1E
#define KEY_3                   0x26
#define KEY_4                   0x25
#define KEY_5                   0x2E
#define KEY_6                   0x36
#define KEY_7                   0x3D
#define KEY_8                   0x3E
#define KEY_9                   0x46
#define KEY_A                   0x1C
#define KEY_B                   0x32
#define KEY_BACKSLASH           0x5D
#define KEY_BACKSPACE           0x66
#define KEY_C                   0x21
#define KEY_CAPS_LOCK           0x58
#define KEY_COLON               0x4C
#define KEY_COMMA               0x41
#define KEY_D                   0x23
#define KEY_DELETE              0x85
#define KEY_DOWN_ARROW          0x8A
#define KEY_E                   0x24
#define KEY_END                 0x88
#define KEY_ENTER               0x5A
#define KEY_EQUAL               0x55
#define KEY_ESCAPE              0x76
#define KEY_F                   0x2B
#define KEY_F1                  0x05
#define KEY_F10                 0x09
#define KEY_F11                 0x78
#define KEY_F12                 0x07
#define KEY_F2                  0x06
#define KEY_F3                  0x04
#define KEY_F4                  0x0C
#define KEY_F5                  0x03
#define KEY_F6                  0x0B
#define KEY_F7                  0x83
#define KEY_F8                  0x0A
#define KEY_F9                  0x01
#define KEY_FORWARD_SLASH       0x4A
#define KEY_G                   0x34
#define KEY_H                   0x33
#define KEY_HOME                0x87
#define KEY_I                   0x43
#define KEY_INSERT              0x81
#define KEY_J                   0x3B
#define KEY_K                   0x42
#define KEY_KP_ASTERISK         0x7C
#define KEY_KP_CENTER           0x73
#define KEY_KP_DELETE           0x71
#define KEY_KP_DOWN_ARROW       0x72
#define KEY_KP_END              0x69
#define KEY_KP_ENTER            0x19
#define KEY_KP_FORWARD_SLASH    0x80
#define KEY_KP_HOME             0x6C
#define KEY_KP_INSERT           0x70
#define KEY_KP_LEFT_ARROW       0x6B
#define KEY_KP_MINUS            0x7B
#define KEY_KP_PAGE_UP          0x7D
#define KEY_KP_PAGE_DOWN        0x7A
#define KEY_KP_PLUS             0x79
#define KEY_KP_RIGHT_ARROW      0x74
#define KEY_KP_UP_ARROW         0x75
#define KEY_L                   0x4B
#define KEY_LEFT_ALT            0x11
#define KEY_LEFT_ARROW          0x86
#define KEY_LEFT_BRACKET        0x54
#define KEY_LEFT_CTRL           0x14
#define KEY_LEFT_SHIFT          0x12
#define KEY_M                   0x3A
#define KEY_MINUS               0x4E
#define KEY_N                   0x31
#define KEY_NUM_LOCK            0x77
#define KEY_O                   0x44
#define KEY_P                   0x4D
#define KEY_PAGE_DOWN           0x8C
#define KEY_PAGE_UP             0x8B
#define KEY_PAUSE               0x82
#define KEY_PERIOD              0x49
#define KEY_PRINT_SCREEN        0x84
#define KEY_Q                   0x15
#define KEY_QUOTE               0x52
#define KEY_R                   0x2D
#define KEY_RIGHT_ALT           0x17
#define KEY_RIGHT_ARROW         0x8D
#define KEY_RIGHT_BRACKET       0x5B
#define KEY_RIGHT_CTRL          0x18
#define KEY_RIGHT_SHIFT         0x59
#define KEY_S                   0x1B
#define KEY_SCROLL_LOCK         0x7E
#define KEY_SPACE               0x29
#define KEY_T                   0x2C
#define KEY_TAB                 0x0D
#define KEY_TILDE               0x0E
#define KEY_U                   0x3C
#define KEY_UP_ARROW            0x89
#define KEY_V                   0x2A
#define KEY_W                   0x1D
#define KEY_X                   0x22
#define KEY_Y                   0x35
#define KEY_Z                   0x1A

TAILQ_HEAD(multi_terminal, smpc_peripheral);

struct smpc_peripheral_analog {
        bool connected;
        /* If no children, port is 1 or 2. Otherwise, port is under multi-terminal */
        uint8_t port_no;
        uint8_t type;
        uint8_t size;

        struct {
                unsigned int right:1;
                unsigned int left:1;
                unsigned int down:1;
                unsigned int up:1;
                unsigned int start:1;
                unsigned int a_trg:1;
                unsigned int c_trg:1;
                unsigned int b_trg:1;

                unsigned int r_trg:1;
                unsigned int x_trg:1;
                unsigned int y_trg:1;
                unsigned int z_trg:1;
                unsigned int l_trg:1;

                uint8_t x;
                uint8_t y;
                uint8_t l_pdl;
                uint8_t r_pdl;
        } __attribute__ ((packed)) button;
} __attribute__ ((packed, __may_alias__));

struct smpc_peripheral_digital {
        bool connected;
        /* If no children, port is 1 or 2. Otherwise, port is under multi-terminal */
        uint8_t port_no;
        uint8_t type;
        uint8_t size;

        struct {
                unsigned int right:1;
                unsigned int left:1;
                unsigned int down:1;
                unsigned int up:1;
                unsigned int start:1;
                unsigned int a_trg:1;
                unsigned int c_trg:1;
                unsigned int b_trg:1;

                unsigned int r_trg:1;
                unsigned int x_trg:1;
                unsigned int y_trg:1;
                unsigned int z_trg:1;
                unsigned int l_trg:1;
        } __attribute__ ((packed)) button;
} __attribute__ ((packed, __may_alias__));

struct smpc_peripheral_info {
        bool connected;
        /* If no children, port is 1 or 2. Otherwise, port is under multi-terminal */
        uint8_t port_no;
        uint8_t type;
        uint8_t size;
        uint8_t data[MAX_PORT_DATA_SIZE]; /* Peripheral data table */
} __attribute__ ((packed));

struct smpc_peripheral {
        struct smpc_peripheral_info info;
        struct smpc_peripheral_port *parent;

        TAILQ_ENTRY(smpc_peripheral) peripherals;
};

struct smpc_peripheral_port {
        struct smpc_peripheral_info info;
        struct multi_terminal *children;
};

extern void smpc_peripheral_data(void);
extern void smpc_peripheral_parse(irq_mux_handle_t *);
extern void smpc_peripheral_system_manager(void);
struct smpc_peripheral_digital *smpc_peripheral_digital_port(uint8_t);

extern struct smpc_peripheral_port smpc_peripheral_port1;
extern struct smpc_peripheral_port smpc_peripheral_port2;

#endif /* !_PERIPHERAL_H_ */
