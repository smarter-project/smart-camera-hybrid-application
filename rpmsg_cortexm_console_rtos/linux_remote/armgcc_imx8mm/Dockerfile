FROM scratch
COPY --from=build /data/cadeniyi/sdk/boards/evkmimx8mm/multicore_examples/rpmsg_cortexm_console_rtos/linux_remote/armgcc/release/rpmsg_cortexm_console_rtos_linux_remote.elf /rpmsg_cortexm_console_rtos_linux_remote.elf
ENTRYPOINT [ "rpmsg_cortexm_console_rtos_linux_remote.elf" ]
