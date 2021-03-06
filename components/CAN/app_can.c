#include "app_can.h"
#include "driver/gpio.h"
#include "CAN_config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define CAN_RX //CAN_TX //

#ifndef CONFIG_ESPCAN
#error for this demo you must enable and configure ESPCan in menuconfig
#endif

#ifdef CONFIG_ESP_CAN_TXD_PIN_NUM
#define CONFIG_SELECTED_CAN_TX_PIN  CONFIG_ESP_CAN_TXD_PIN_NUM
#endif

#ifdef CONFIG_ESP_CAN_RXD_PIN_NUM
#define CONFIG_SELECTED_CAN_RX_PIN  CONFIG_ESP_CAN_TXD_PIN_NUM
#endif

#ifdef CONFIG_CAN_SPEED_100KBPS
#define CONFIG_SELECTED_CAN_SPEED CAN_SPEED_100KBPS
#endif

#ifdef CONFIG_CAN_SPEED_125KBPS
#define CONFIG_SELECTED_CAN_SPEED CAN_SPEED_125KBPS
#endif

#ifdef CONFIG_CAN_SPEED_250KBPS
#define CONFIG_SELECTED_CAN_SPEED CAN_SPEED_250KBPS
#endif

#ifdef CONFIG_CAN_SPEED_500KBPS
#define CONFIG_SELECTED_CAN_SPEED CAN_SPEED_500KBPS
#endif

#ifdef CONFIG_CAN_SPEED_800KBPS
#define CONFIG_SELECTED_CAN_SPEED CAN_SPEED_800KBPS
#endif

#ifdef CONFIG_CAN_SPEED_1000KBPS
#define CONFIG_SELECTED_CAN_SPEED CAN_SPEED_1000KBPS
#endif

#ifdef CONFIG_CAN_SPEED_USER_KBPS
#define CONFIG_SELECTED_CAN_SPEED CONFIG_CAN_SPEED_USER_KBPS_VAL /* per menuconfig */
#endif

CAN_device_t CAN_cfg;

void app_can_setup()
{
    CAN_cfg.speed = CONFIG_SELECTED_CAN_SPEED;
    CAN_cfg.tx_pin_id = CONFIG_SELECTED_CAN_TX_PIN;
    CAN_cfg.rx_pin_id = CONFIG_SELECTED_CAN_RX_PIN;
    CAN_cfg.rx_queue = xQueueCreate(10,sizeof(CAN_frame_t));
    CAN_init();
}

void CAN_poll_task(void *ignore)
{
#ifdef CAN_TX
    printf("%s ::CAN_TX run...\r\n",__func__);
    CAN_frame_t tx_frame;
    tx_frame.MsgID = 1;
    tx_frame.FIR.B.DLC = 8;
    tx_frame.data.u8[0] = 'W';
    tx_frame.data.u8[1] = 'E';
    tx_frame.data.u8[2] = '_';
    tx_frame.data.u8[3] = 'L';
    tx_frame.data.u8[4] = 'I';
    tx_frame.data.u8[5] = 'N';
    tx_frame.data.u8[6] = 'K';
    tx_frame.data.u8[7] = '.';

    while(true){
        app_can_send(&tx_frame);
        vTaskDelay( 2000/ portTICK_PERIOD_MS);
        if(tx_frame.MsgID > 256)
            tx_frame.MsgID = 1;
        tx_frame.MsgID ++;
    }

#elif defined CAN_RX
    printf("%s ::CAN_RX run...\r\n",__func__);
    CAN_frame_t rx_frame;
    while(true){
        if(pdTRUE == xQueueReceive(CAN_cfg.rx_queue, &rx_frame, 3*portTICK_PERIOD_MS))
        {
            if(rx_frame.FIR.B.FF == CAN_frame_std)
                printf("New standard frame # ");
            else 
                printf("New extended frame # ");

            if(rx_frame.FIR.B.RTR == CAN_RTR)
                printf(" RTR from 0x%08x, DLC %d\r\n", rx_frame.MsgID, rx_frame.FIR.B.DLC);
            else
                printf(" from 0x%08x, DLC %d, dataL: 0x%08x, dataH: 0x%08x \r\n",
                    rx_frame.MsgID, rx_frame.FIR.B.DLC, rx_frame.data.u32[0], rx_frame.data.u32[1]);
                
        }
        
    }

#endif

}
#if 0
void CAN_poll_task(void *ignore)
{
    printf("%s run...\r\n",__func__);
    CAN_frame_t rx_frame;
    while(true){
        //receive next CAN frame from queue
        if(xQueueReceive(CAN_cfg.rx_queue,&rx_frame, 3*portTICK_PERIOD_MS)==pdTRUE){

            //do stuff!
            if(rx_frame.FIR.B.FF==CAN_frame_std)
                printf("New standard frame");
            else
                printf("New extended frame");

            if(rx_frame.FIR.B.RTR==CAN_RTR)
                printf(" RTR from 0x%08x, DLC %d\r\n",rx_frame.MsgID,  rx_frame.FIR.B.DLC);
            else{
                printf(" from 0x%08x, DLC %d\n",rx_frame.MsgID,  rx_frame.FIR.B.DLC);
                /* convert to upper case and respond to sender */
                for(int i = 0; i < 8; i++){
                    if(rx_frame.data.u8[i] >= 'a' && rx_frame.data.u8[i] <= 'z'){
                        rx_frame.data.u8[i] = rx_frame.data.u8[i] - 32;
                    }
                }
            }
        }
    }
}
#endif


//int CAN_write_frame(const CAN_frame_t* p_frame);

int app_can_send(const CAN_frame_t* p_frame)
{
    return CAN_write_frame(p_frame);
}

int app_can_stop()
{
    return CAN_stop();
}



