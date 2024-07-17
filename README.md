# Running the SMARTER Demo on a Heterogeneous platform


The Evolving Edge whitepaper, https://armkeil.blob.core.windows.net/developer/Files/pdf/white-paper/arm-evolving-edge-computing-white-paper.pdf, highlights heterogeneity as an essential ingredient of Evolving Edge Computing. We call systems that contain more than one different type of processor a "hybrid system". The "Cloud-Native Deployment on Hybrid Systems" blog describes the implementation of a container runtime that enables the deployment of applications onto additional processors within a system.  For example, this can be used to deploy applications onto a microcontroller embedded alongside application-profile cores in an SoC.

In this project we show how this new container runtime can be used to extend an existing application that is deployed using k3s (a container orchestrator) onto an edge compute node based on a hybrid SoC.

## SMARTER Project

As part of the SMARTER (Secure Municipal, Agricultural, Rural, and Telco Edge Research) project, Arm has been exploring the use of cloud-native technology and methodologies in edge environments to evaluate their effectiveness at addressing the challenges of developing, debugging, depolying and securing software for edge devices. The paper "SMARTER: Experiences with Cloud Native on the Edge" [HotEdge] presented at the HotEdge'20 workshop on Hot Topics in Edge Computing describes in more detail how we address the challenges, especially at scale and the code is available via the project website https://getsmarter.io.https. 


The SMARTER Demo application consists of a number of containerized services that are deployed onto an edge system using a container orchestrator (k3s) that has been configured to cater for the differences between deploying to the edge and deploying to the cloud. Once deployed the demo application uses a machine learning model to perform object detection on a video stream. Detection data is sent to the cloud where it can be visualized via a grafana dashabord. 

![Smarter Demo Application][demoapp]

*SMARTER Demo Application*

## Hardware platform

For this demo we are using the iMX8M-Mini-EVK [IMX8MMEVK] from NXP. The i.MX8M MINI LPDDR4 EVK board is a platform designed to show the most commonly used features of the i.MX 8M Mini Applications Processor in a small and low-cost package. The i.MX 8M Mini Quad applications processor [SoC] contains 4x Cortex-A53 @ 1.8 GHz and 1x Cortex-M4 @ 400 MHz.  The EVK board has 2GB LPDDR4, 16GB eMMC and 32GB QSPI NOR. 

## Software

We are running a Yocto Linux built using the instructions on the NXP website. We built using the   Micklecore release and a 6.1.22 kernel version. We configured the kernel to enable the use of containers and k3s.

The SMARTER application is designed to take input from a camera but for the purpose of demonstration we use an MP4 video file and configure the application to use it. Also for the purposes of our demo we are not sending any data to the cloud and we do not use the audio processing part of the application. 

### Hybrid Application 
We have added a new part of the application which runs on the Cortex-M4 and deployed using k3s just like the other parts of the demo application. The new functionality is contained in a FreeRTOS application that monitors the status of the Cortex-A53 cores ie whether they are running or have been put into sleep mode. If the application detects that the Cortex-A53 cores have been put into sleep mode then it will wait for ten seconds and then wake the Cortex-A53 cores via a specific interrupt. This allows us to show how an embedded MCU could be used to perform a task that requires lower performance whilst the main system is suspended (which can save energy) and then wake the system if more compute power is required.


![Hybrid Smarter Demo Application][hybriddemoapp]

*SMARTER Demo Application with a hybrid component*


## Video Demonstration 

The accompanying video: [link] shows how we can deploy the SMARTER application
to the iMX8M-Mini-EVK. We follow the instructions given in the SMARTER Project
repo [DOCS] and then deploy an extra component that runs on the Cortex-M4.

The scripts and files used in the video demonstration are included in this repo


## References


[HotEdge]:  https://www.usenix.org/system/files/hotedge20_paper_ferreira_0.pdf "HotEdge '20 paper"

[IMX8MMEVK]: https://www.nxp.com/design/design-center/development-boards-and-designs/i-mx-evaluation-and-development-boards/evaluation-kit-for-the-i-mx-8m-mini-applications-processor:8MMINILPD4-EVK "IMX8M-MINI-EVK"

[SoC]: https://www.nxp.com/assets/block-diagram/en/i.MX8MMINI.pdf " IMX8M-MINI SoC"

[DOCS]: https://github.com/smarter-project/documentation "SMARTER"

[demoapp]: images/smarter.drawio.png "SMARTER Demo Application"

[hybriddemoapp]: images/hybrid_smarter.drawio.png "Hybrid SMARTER Demo Application"


[HotEdge]

[IMX8MMEVK]

[SoC]

[DOCS]




