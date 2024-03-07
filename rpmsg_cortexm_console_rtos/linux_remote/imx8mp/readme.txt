Overview
========
This example uses RPMSG (via shared memory) to provide a way to create a console
log file readable from the Cortex-A53s in the SoC.

Shared memory usage
This multicore example uses the shared memory for data exchange. The shared memory region is
defined and the size can be adjustable in the linker file. The shared memory region start address
and the size have to be defined in linker file for each core equally. The shared memory start
address is then exported from the linker to the application.

Toolchain supported
===================
- GCC ARM Embedded  12.2
- IAR embedded Workbench  9.40.1


Building
========

The directory should be put within the MCUXEpresso SDK example directory:
  for example under .../sdk/boards/evkmimx8mm/multicore_examples

The ARMGCC_DIR needs to be set to point to the tools:
  for example:  ARMGCC_DIR=../arm-gnu-toolchain-13.2.Rel1-x86_64-arm-none-eabi

cd .../sdk/boards/evkmimx8mm/multicore_examples/rpmsg_cortexm_console_rtos/linux_remote/armgcc

./build_release.sh


The final firmware file should be created in: release/rpmsg_cortexm_console_rtos_linux_remote.elf


Execution
=========

This file can be copied into the rootfilsystem for the imx8m-mini-evk under /lib/firmware

Remoteproc can be used to load and start the firmware



