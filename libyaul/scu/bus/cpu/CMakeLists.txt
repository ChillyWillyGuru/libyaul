SET(scu_bus_cpu_SRC
  cpu_dmac_channel_set.c
  cpu_dmac_channel_start.c
  cpu_dmac_channel_stop.c
  cpu_intc_disable.S
  cpu_intc_enable.S
  cpu_intc_interrupt_get.S
  cpu_intc_interrupt_set.S
  cpu_intc_mask_get.S
  cpu_intc_mask_set.S
  cpu_intc_vector_base_get.S
  cpu_intc_vector_base_set.S
  cpu_intc_vector_table.S
  cpu_ubc_init.c
)

SET(scu_bus_cpu_INSTALL_FILES
  cpu.h
  cpu/dmac.h
  cpu/intc.h
  cpu/ubc.h
)
