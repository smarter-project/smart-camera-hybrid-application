/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rpmsg_lite.h"
#include "rpmsg_queue.h"
#include "rpmsg_ns.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_mu.h"
#include "FreeRTOS.h"
#include "task.h"

#include "fsl_uart.h"
#include "rsc_table.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#ifdef IMX8MM
#define RPMSG_LITE_LINK_ID            (RL_PLATFORM_IMX8MM_M4_USER_LINK_ID)
#endif

#ifdef IMX8MP
#define RPMSG_LITE_LINK_ID            (RL_PLATFORM_IMX8MP_M7_USER_LINK_ID)
#endif

#ifndef RPMSG_LITE_LINK_ID
#error "SoC not specified"
#endif

#define RPMSG_LITE_SHMEM_BASE         (VDEV0_VRING_BASE)
#define RPMSG_LITE_NS_ANNOUNCE_STRING "rpmsg-openamp-demo-channel"
#define RPMSG_LITE_MASTER_IS_LINUX

#define APP_DEBUG_UART_BAUDRATE (115200U) /* Debug console baud rate. */
#define APP_TASK_STACK_SIZE (256U)
#ifndef LOCAL_EPT_ADDR
#define LOCAL_EPT_ADDR (30U)
#endif
#define APP_RPMSG_READY_EVENT_DATA (1U)

typedef struct the_message
{
    uint32_t DATA;
} THE_MESSAGE, *THE_MESSAGE_PTR;

static volatile THE_MESSAGE msg = {0};
#ifdef RPMSG_LITE_MASTER_IS_LINUX
static char helloMsg[32];
#endif /* RPMSG_LITE_MASTER_IS_LINUX */


static char msg_buf[512];

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

#define rprintf(...) sprintf(msg_buf, __VA_ARGS__); rpmsg_printf(msg_buf);

/*******************************************************************************
 * Code
 ******************************************************************************/
static TaskHandle_t app_task_handle = NULL;

static struct rpmsg_lite_instance *volatile my_rpmsg = NULL;

static struct rpmsg_lite_endpoint *volatile my_ept = NULL;
static volatile rpmsg_queue_handle my_queue        = NULL;

volatile uint32_t remote_addr;

void app_destroy_task(void)
{
    if (app_task_handle)
    {
        vTaskDelete(app_task_handle);
        app_task_handle = NULL;
    }

    if (my_ept)
    {
        rpmsg_lite_destroy_ept(my_rpmsg, my_ept);
        my_ept = NULL;
    }

    if (my_queue)
    {
        rpmsg_queue_destroy(my_rpmsg, my_queue);
        my_queue = NULL;
    }

    if (my_rpmsg)
    {
        rpmsg_lite_deinit(my_rpmsg);
        my_rpmsg = NULL;
    }
}

static void app_nameservice_isr_cb(uint32_t new_ept, const char *new_ept_name, uint32_t flags, void *user_data)
{
}

int A53_asleep() {
  return  (MU_GetOtherCorePowerMode(MUB)==3);
}

int A53_awake() {
  return  (MU_GetOtherCorePowerMode(MUB)==0);
}

void rpmsg_printf(char *msg) {
  uint32_t len;
  /* Only send into to A53 if they are awake! */
  if (!A53_asleep()) {
    len = strlen(msg);
    (void)rpmsg_lite_send(my_rpmsg, my_ept, remote_addr, msg, len, RL_BLOCK);
  }
  PRINTF(msg);
}


static void app_task(void *param)
{

    volatile rpmsg_ns_handle ns_handle;

#ifdef IMX8MM    
    const TickType_t xDelay = 1000 / portTICK_PERIOD_MS;
#else    
    const TickType_t xDelay = 33;
#endif
    
    static int i = 0;
    static int countdown = 0;
    static bool wakeup = false;
    
    /* Print the initial banner */
    (void)PRINTF("\r\nSMARTER Hybrid application\r\n");
    
#ifdef MCMGR_USED
    uint32_t startupData;
    mcmgr_status_t status;

    /* Get the startup data */
    do
    {
        status = MCMGR_GetStartupData(&startupData);
    } while (status != kStatus_MCMGR_Success);

    my_rpmsg = rpmsg_lite_remote_init((void *)(char *)startupData, RPMSG_LITE_LINK_ID, RL_NO_FLAGS);

    /* Signal the other core we are ready by triggering the event and passing the APP_RPMSG_READY_EVENT_DATA */
    (void)MCMGR_TriggerEvent(kMCMGR_RemoteApplicationEvent, APP_RPMSG_READY_EVENT_DATA);
#else
    /* (void)PRINTF("RPMSG Share Base Addr is 0x%x\r\n", RPMSG_LITE_SHMEM_BASE); */
    my_rpmsg = rpmsg_lite_remote_init((void *)RPMSG_LITE_SHMEM_BASE, RPMSG_LITE_LINK_ID, RL_NO_FLAGS);
#endif /* MCMGR_USED */
    (void)PRINTF("Waiting for link to be established\r\n");
    rpmsg_lite_wait_for_link_up(my_rpmsg, RL_BLOCK);
    (void)PRINTF("Link is up!\r\n");

    my_queue  = rpmsg_queue_create(my_rpmsg);
    my_ept    = rpmsg_lite_create_ept(my_rpmsg, LOCAL_EPT_ADDR, rpmsg_queue_rx_cb, my_queue);
    ns_handle = rpmsg_ns_bind(my_rpmsg, app_nameservice_isr_cb, ((void *)0));
    /* Introduce some delay to avoid NS announce message not being captured by the master side.
       This could happen when the remote side execution is too fast and the NS announce message is triggered
       before the nameservice_isr_cb is registered on the master side. */
    SDK_DelayAtLeastUs(1000000U, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
    (void)rpmsg_ns_announce(my_rpmsg, my_ept, RPMSG_LITE_NS_ANNOUNCE_STRING, (uint32_t)RL_NS_CREATE);
    (void)PRINTF("Nameservice announce sent.\r\n");

#ifdef RPMSG_LITE_MASTER_IS_LINUX
    /* Wait Hello handshake message from Remote Core. */
    (void)PRINTF("Waiting for handshake from remote core\r\n");    
    (void)rpmsg_queue_recv(my_rpmsg, my_queue, (uint32_t *)&remote_addr, helloMsg, sizeof(helloMsg), ((void *)0),
                           RL_BLOCK);
    (void)PRINTF("Received Handshake from remote core\r\n");        
#endif /* RPMSG_LITE_MASTER_IS_LINUX */

    for (;;) {

      rprintf("Timestamp: %d\r\n", i);
      //      PRINTF("MUB: %d\r\n", MU_GetOtherCorePowerMode(MUB));      

      if ( (i % 15 == 0) && (countdown == 0)) {
        if (A53_asleep()) { // Cortex A is sleeping
          PRINTF("A53s are asleep\r\n");
          PRINTF("Let's wake them up in 10 seconds!\r\n");
          wakeup = true;
          countdown=10;
        }
      }
      if ((wakeup==true) && (countdown > 0)) {
        countdown--;
        if (countdown == 0) {
          PRINTF("Waking up\r\n");
          MU_SendMsg(MUB, 1, 2); //wake up Cortex A
          wakeup = false;
        }
      }
      i++;
      vTaskDelay( xDelay );
    }
    
    
    (void)PRINTF("Done, deinitializing...\r\n");
    
    (void)rpmsg_lite_destroy_ept(my_rpmsg, my_ept);
    my_ept = ((void *)0);
    (void)rpmsg_queue_destroy(my_rpmsg, my_queue);
    my_queue = ((void *)0);
    (void)rpmsg_ns_unbind(my_rpmsg, ns_handle);
    (void)rpmsg_lite_deinit(my_rpmsg);
    my_rpmsg = ((void *)0);
    msg.DATA = 0U;
    
}

void app_create_task(void)
{
    if (app_task_handle == NULL &&
        xTaskCreate(app_task, "APP_TASK", APP_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, &app_task_handle) != pdPASS)
    {
        PRINTF("\r\nFailed to create application task\r\n");
        for (;;)
            ;
    }
}

/*!
 * @brief Main function
 */
int main(void)
{
    /* Initialize standard SDK demo application pins */
    /* Board specific RDC settings */
    BOARD_RdcInit();

    BOARD_InitBootPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    BOARD_InitMemory();
    MU_Init(MUB);
    
    copyResourceTable();

#ifdef MCMGR_USED
    /* Initialize MCMGR before calling its API */
    (void)MCMGR_Init();
#endif /* MCMGR_USED */

    app_create_task();
    vTaskStartScheduler();

    (void)PRINTF("Failed to start FreeRTOS on core0.\r\n");
    for (;;)
    {
    }
}
