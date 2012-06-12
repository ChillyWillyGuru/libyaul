/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _USB_CARTRIDGE_H__
#define _USB_CARTRIDGE_H__

#include <inttypes.h>
#include <stdbool.h>

extern uint32_t usb_cartridge_read_long(void);
extern uint8_t usb_cartridge_read_byte(void);
extern uint8_t usb_cartridge_xchg_byte(uint8_t);
extern void usb_cartridge_init(void);
extern void usb_cartridge_send_byte(uint8_t);
extern void usb_cartridge_send_long(uint32_t);

#endif /* !_USB_CARTRIDGE_H_ */
