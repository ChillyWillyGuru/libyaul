SET(scu_bus_cpu_smpc_SRC
  smpc_init.c
  smpc_peripheral_analog_port.c
  smpc_peripheral_data.c
  smpc_peripheral_digital_port.c
  smpc_peripheral_keyboard_port.c
  smpc_peripheral_mouse_port.c
  smpc_peripheral_parse.c
  smpc_peripheral_racing_port.c
  smpc_peripheral_raw_port.c
  smpc_peripheral_system_manger.c
  smpc_rtc_settime_call.c
  smpc_smc_call.c
  smpc_smc_cdoff_call.c
  smpc_smc_cdon_call.c
  smpc_smc_ckchg320_call.c
  smpc_smc_ckchg352_call.c
  smpc_smc_intback_call.c
  smpc_smc_mshon_call.c
  smpc_smc_nmireq_call.c
  smpc_smc_resdisa_call.c
  smpc_smc_resenab_call.c
  smpc_smc_setsmem_call.c
  smpc_smc_sndoff_call.c
  smpc_smc_sndon_call.c
  smpc_smc_sshoff_call.c
  smpc_smc_sshon_call.c
  smpc_smc_sysres_call.c
)

SET(scu_bus_cpu_smpc_INSTALL_FILES
  smpc.h
  smpc/peripheral.h
  smpc/rtc.h
  smpc/smc.h
)
