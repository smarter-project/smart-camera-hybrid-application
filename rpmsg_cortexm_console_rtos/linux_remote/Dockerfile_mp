FROM ubuntu:22.04 as buildenv

RUN apt update && \
    apt install --no-install-recommends -y \
    build-essential \
    cmake \
    gcc-arm-none-eabi \
    libnewlib-arm-none-eabi \
    libnewlib-dev \
    libstdc++-arm-none-eabi-dev \
    libstdc++-arm-none-eabi-newlib

RUN apt install -y git
RUN apt install -y python3 python3-pip
RUN pip3 install west


RUN mkdir /mcuxsdk
WORKDIR /mcuxsdk
RUN  west init -m https://github.com/NXPmicro/mcux-sdk --mr MCUX_2.15.000 /mcuxsdk
RUN west update
     
RUN mkdir /app
WORKDIR /app

COPY imx8mp /app/imx8mp
COPY armgcc_imx8mp /app/armgcc_imx8mp
COPY main_remote.c /app

WORKDIR /app/armgcc_imx8mp
RUN export ARMGCC_DIR=/usr/ && ./build_release.sh

FROM scratch as firmware
COPY --from=buildenv /app/armgcc_imx8mp/release/rpmsg_cortexm_console_rtos_linux_remote.elf  /rpmsg_cortexm_console_rtos_linux_remote.elf
ENTRYPOINT [ "/rpmsg_cortexm_console_rtos_linux_remote.elf" ]
LABEL board="FSL i.MX8MPlus EVK board" mcu="imx-rproc"
