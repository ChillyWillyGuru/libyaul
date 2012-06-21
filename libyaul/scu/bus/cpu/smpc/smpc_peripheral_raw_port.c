/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <smpc/peripheral.h>

#include "smpc-internal.h"

struct smpc_peripheral_port *
smpc_peripheral_raw_port(uint8_t port)
{

        switch (port) {
        case 1:
                return &smpc_peripheral_port1;
        case 2:
                return &smpc_peripheral_port2;
        default:
                return NULL;
        }
}
