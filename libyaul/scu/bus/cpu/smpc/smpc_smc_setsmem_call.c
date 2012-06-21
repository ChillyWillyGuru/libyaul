/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include "smpc-internal.h"

/* Command type: Non-resetable system management commands */
uint8_t
smpc_smc_setsmem_call(void)
{

        smpc_cmd_call(SMPC_SMC_SETSMEM, SMPC_CMD_ISSUE_TYPE_C, NULL);

        return MEMORY_READ(8, OREG(31));
}
