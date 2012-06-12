/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include "smpc_internal.h"

uint8_t
smpc_cmd_sysres_call(void)
{
        /* Completely soft reboot the entire system. */
        smpc_cmd_call(SMPC_SMC_SYSRES, SMPC_CMD_ISSUE_TYPE_A, NULL);

        return MEM_READ(OREG(31));
}
