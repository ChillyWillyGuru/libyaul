/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _SMC_H_
#define _SMC_H_

#include <inttypes.h>

extern uint8_t smpc_smc_cdoff_call(void);
extern uint8_t smpc_smc_cdon_call(void);
extern uint8_t smpc_smc_ckchg320_call(void);
extern uint8_t smpc_smc_ckchg352_call(void);
extern uint8_t smpc_smc_intback_call(uint8_t, uint8_t);
extern uint8_t smpc_smc_mshon_call(void);
extern uint8_t smpc_smc_nmireq_call(void);
extern uint8_t smpc_smc_resdisa_call(void);
extern uint8_t smpc_smc_resenab_call(void);
extern uint8_t smpc_smc_setsmem_call(void);
extern uint8_t smpc_smc_sndoff_call(void);
extern uint8_t smpc_smc_sndon_call(void);
extern uint8_t smpc_smc_sshoff_call(void);
extern uint8_t smpc_smc_sshon_call(void);
extern uint8_t smpc_smc_sysres_call(void);

#endif /* !_SMC_H_ */
