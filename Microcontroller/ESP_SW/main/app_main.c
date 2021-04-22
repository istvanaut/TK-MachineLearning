/* SPI Slave example, receiver (uses SPI Slave driver to communicate with sender)

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "lwip/igmp.h"

#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "soc/rtc_periph.h"
#include "driver/spi_slave.h"
#include "esp_log.h"
#include "esp_spi_flash.h"
#include "driver/gpio.h"




/*
SPI receiver (slave) example.

This example is supposed to work together with the SPI sender. It uses the standard SPI pins (MISO, MOSI, SCLK, CS) to
transmit data over in a full-duplex fashion, that is, while the master puts data on the MOSI pin, the slave puts its own
data on the MISO pin.

This example uses one extra pin: GPIO_HANDSHAKE is used as a handshake pin. After a transmission has been set up and we're
ready to send/receive data, this code uses a callback to set the handshake pin high. The sender will detect this and start
sending a transaction. As soon as the transaction is done, the line gets set low again.
*/

/*
Pins in use. The SPI Master can use the GPIO mux, so feel free to change these if needed.
*/
#define GPIO_HANDSHAKE 2
#define GPIO_MOSI 12
#define GPIO_MISO 13
#define GPIO_SCLK 15
#define GPIO_CS 14

#ifdef CONFIG_IDF_TARGET_ESP32
#define RCV_HOST    HSPI_HOST
#define DMA_CHAN    2

#elif defined CONFIG_IDF_TARGET_ESP32S2
#define RCV_HOST    SPI2_HOST
#define DMA_CHAN    RCV_HOST

#endif
// define transmission states
#define STATE_INIT                        1u
#define STATE_WAITING_FOR_REQUEST         2u
#define STATE_SENDING_RESPONSE            3u

// define response constants
#define RESP_OK    0x01u
#define RESP_NOK   0x00u

// define requests
#define REQ_WEIGHTS                 0x01u
#define REQ_CAMERA_PIC              0x02u
#define REQ_SEND_STATE_1            0x03u
#define REQ_SEND_STATE_2            0x04u
#define REQ_PING                    0X05u

#define CAM_WIDTH   32
#define CAM_HEIGHT  32

#define WEIGHTS_SIZE     4848

typedef struct
{
	/* Previous sensor states*/
	uint32_t lightsensorP;
	uint32_t usLeftP;
	uint32_t usRightP;
	uint32_t laserP;
	/* Performed previous action*/
	float leftM;
	float rightM;
    float reward;

}sensorState;
typedef struct {
    /* Current reward based on the previous states and action*/
    /* Current states*/
    uint32_t lightsensorC;
	uint32_t usLeftC;
	uint32_t usRightC;
	uint32_t laserC;
    /* data */
}sensorStateC;
union sensorStatePU
{
    sensorState* statesP;
    uint8_t* buf;
};
union sensorStateCU
{
    sensorStateC* statesC;
    uint8_t* buf;
};

//Called after a transaction is queued and ready for pickup by master. We use this to set the handshake line high.
void my_post_setup_cb(spi_slave_transaction_t *trans) {
    WRITE_PERI_REG(GPIO_OUT_W1TS_REG, (1<<GPIO_HANDSHAKE));
}

//Called after transaction is sent/received. We use this to set the handshake line low.
void my_post_trans_cb(spi_slave_transaction_t *trans) {
    WRITE_PERI_REG(GPIO_OUT_W1TC_REG, (1<<GPIO_HANDSHAKE));
}

uint8_t* camera_frame;

//Main application
void app_main(void)
{
    printf("App_main started");
    int n=0;
    esp_err_t ret;
    uint8_t trans_state = STATE_INIT;
    uint8_t trans_nextstate = STATE_INIT;
    uint8_t request = 0u;
    bool skip = false;
    //Configuration for the SPI bus
    spi_bus_config_t buscfg={
        .mosi_io_num=GPIO_MOSI,
        .miso_io_num=GPIO_MISO,
        .sclk_io_num=GPIO_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz=WEIGHTS_SIZE+4
    };

    //Configuration for the SPI slave interface
    spi_slave_interface_config_t slvcfg={
        .mode=1,
        .spics_io_num=GPIO_CS,
        .queue_size=3,
        .flags=0,
        .post_setup_cb=my_post_setup_cb,
        .post_trans_cb=my_post_trans_cb
    };

    //Configuration for the handshake line
    gpio_config_t io_conf={
        .intr_type=GPIO_INTR_DISABLE,
        .mode=GPIO_MODE_OUTPUT,
        .pin_bit_mask=(1<<GPIO_HANDSHAKE)
    };

    //Configure handshake line as output
    gpio_config(&io_conf);
    //Enable pull-ups on SPI lines so we don't detect rogue pulses when no master is connected.
    gpio_set_pull_mode(GPIO_MOSI, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(GPIO_SCLK, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(GPIO_CS, GPIO_PULLUP_ONLY);
    printf("Staring Initializing\n");
    //Initialize SPI slave interface
    ret=spi_slave_initialize(RCV_HOST, &buscfg, &slvcfg, DMA_CHAN);
    assert(ret==ESP_OK);
    printf("Initialized\n");
    WORD_ALIGNED_ATTR uint8_t sendbuf[129]="";
    WORD_ALIGNED_ATTR uint8_t recvbuf[129]="";
    memset(recvbuf, 0, 33);
    spi_slave_transaction_t t;
    memset(&t, 0, sizeof(t));
    trans_state = STATE_WAITING_FOR_REQUEST;
    trans_nextstate = STATE_WAITING_FOR_REQUEST;

    /*Define DMA buffers in DMA capable memory*/
    uint8_t *weights_dma = (uint8_t *)heap_caps_malloc(WEIGHTS_SIZE, MALLOC_CAP_DMA);
    //uint8_t *sensorStates_dma = (uint8_t *)heap_caps_malloc(11*sizeof(uint32_t),MALLOC_CAP_DMA);
    camera_frame = (uint8_t *)heap_caps_malloc(CAM_WIDTH*CAM_HEIGHT, MALLOC_CAP_DMA);

    union sensorStateCU dataC;
    union sensorStatePU dataP;
    sensorState statePrev;
    sensorStateC stateCurr;
    //memcpy(weights_dma,weights,sizeof(weights));
    printf("Sizeu: %u\n", sizeof(uint32_t));
    printf("Sizes: %u\n", sizeof(sensorState));
    printf("Sizew: %u\n", sizeof(weights_dma));
    for (int i =0; i<WEIGHTS_SIZE;i++)
    {
        weights_dma[i] = 0xAA;
    }
    for (int i =0; i<CAM_WIDTH*CAM_HEIGHT;i++)
    {
        camera_frame[i] = 0x7C;
    }
    camera_frame[0] = 0xEA;
    camera_frame[CAM_WIDTH*CAM_HEIGHT-1] = 0xDB;
    weights_dma[1] = 0xEE;
    weights_dma[2500] = 0xCE;
    weights_dma[4847] = 0xED;
    
    
    while(1) {
        //Clear receive buffer, set send buffer to something sane
        skip = false;
        printf("---------------------------------------------------------------");
        switch (trans_state)
        {
        case STATE_WAITING_FOR_REQUEST:
            printf("\nSTATE_WAITING_FOR_REQUEST\n");
            printf("Waiting for data...\n");
            //Set up a transaction of 1 byte to receive
            t.length=8;
            t.tx_buffer=NULL;
            t.rx_buffer=recvbuf;
            memset(recvbuf, 0xA5, 129);
            trans_nextstate = STATE_SENDING_RESPONSE;
            break;
        case STATE_SENDING_RESPONSE:
            printf("\nSTATE_SENDING_RESPONSE\n");
            printf("Waiting for transmission...\n");
            memset(sendbuf, 0xA5, 129);
            switch (request)
            {
            case REQ_PING:
                printf("REQ_PING qn");
                t.length=8;
                sendbuf[0] = RESP_OK;
                t.tx_buffer = sendbuf;
                t.rx_buffer = NULL;
                break;
            case REQ_WEIGHTS:
                printf("REQ_WEIGHTS\n");
                t.length= 8 * WEIGHTS_SIZE;
                t.tx_buffer = weights_dma;
                t.rx_buffer = NULL;
                break;
            case REQ_SEND_STATE_1:
                printf("REQ_SEND_STATE_1\n");
                t.length = 8 * 7*sizeof(uint32_t);
                //memset(sensorStates_dma, 0xA5, 11*sizeof(uint32_t));
                t.tx_buffer=NULL;
                t.rx_buffer=recvbuf;
                dataP.buf = recvbuf;
                memset(recvbuf, 0xA5, 129);
                break;
            case REQ_SEND_STATE_2:
                printf("REQ_SEND_STATE_2\n");
                t.length = 8 * 4*sizeof(uint32_t);
                //memset(sensorStates_dma, 0xA5, 11*sizeof(uint32_t));
                t.tx_buffer=NULL;
                t.rx_buffer=recvbuf;
                dataC.buf = recvbuf;
                memset(recvbuf, 0xA5, 129);
                break;
            case REQ_CAMERA_PIC:
                printf("REQ_CAMERA_PIC\n");
                /* Remember to use Semaphore here when integrating!!!*/
                if(camera_frame[0] == 0xEA) printf("Equals");
                t.length = 8 * CAM_WIDTH*CAM_HEIGHT;
                t.tx_buffer = camera_frame;
                t.rx_buffer = NULL;
                break;
            default:
                // useless data received presumably, so go back to request state
                printf("DEFAULT REQUEST handler, skipping\n");
                skip = true;
                t.length=8;
                sendbuf[0] = RESP_NOK;
                t.tx_buffer=sendbuf;
                t.rx_buffer=NULL;
                break;
            }
            
            
            trans_nextstate = STATE_WAITING_FOR_REQUEST;
            break;
        default:
            break;
        }
        
        
        //sprintf(sendbuf, "This is the receiver, sending data for transmission number %04d.", n);


        /* This call enables the SPI slave interface to send/receive to the sendbuf and recvbuf. The transaction is
        initialized by the SPI master, however, so it will not actually happen until the master starts a hardware transaction
        by pulling CS low and pulsing the clock etc. In this specific example, we use the handshake line, pulled up by the
        .post_setup_cb callback that is called as soon as a transaction is ready, to let the master know it is free to transfer
        data.
        */
        if(!skip)
        {
            ret=spi_slave_transmit(RCV_HOST, &t, portMAX_DELAY);

            //spi_slave_transmit does not return until the master has done a transmission, so by here we have sent our data and
            //received data from the master. Print it.
            if(t.rx_buffer != NULL)
            {
                if(trans_state == STATE_WAITING_FOR_REQUEST)
                {
                    printf("Received: %s\n", recvbuf); 
                    request=recvbuf[0];
                }
                else
                {
                    if(request == REQ_SEND_STATE_1 && trans_state == STATE_SENDING_RESPONSE)
                    {
                        
                        printf("Received: %s\n", recvbuf); 
                        printf("\n-----------STATES_PREV---------\n");
                        printf("lightsensor value Previous: %u\n", dataP.statesP->lightsensorP);
                        printf("usLeft value Previous: %u\n", dataP.statesP->usLeftP);
                        printf("usRight value Previous: %u\n", dataP.statesP->usRightP);
                        printf("laser value Previous: %u\n", dataP.statesP->laserP);
                        printf("leftM value Previous: %g\n", dataP.statesP->leftM);
                        printf("rightM sensor value Previous: %g\n", dataP.statesP->rightM);
                        printf("reward sensor value: %g\n", dataP.statesP->reward);
                        
                    }
                    if(request == REQ_SEND_STATE_2 && trans_state == STATE_SENDING_RESPONSE)
                    {
                        
                        printf("Received: %s\n", recvbuf); 
                        printf("\n-----------STATES_CURR---------\n");
                        
                        printf("lightsensor value Current: %u\n", dataC.statesC->lightsensorC);
                        printf("usLeft value Current: %u\n", dataC.statesC->usLeftC);
                        printf("usRight value Current: %u\n", dataC.statesC->usRightC);
                        printf("laser value Current: %u\n", dataC.statesC->laserC);
                    }
                }
                
                
            } 
            if(t.tx_buffer != NULL) printf("Transmitted: %s\n", sendbuf);
            
        }
        
        
        //Update the state
        trans_state = trans_nextstate;
        n++;
        vTaskDelay(20);
    }

}


