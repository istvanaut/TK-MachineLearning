/* BSD Socket API Example

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

#include <stdlib.h>

#include "driver/sdmmc_host.h"
#include "driver/sdmmc_defs.h"
#include "sdmmc_cmd.h"
#include "esp_vfs_fat.h"

#include "esp_camera.h"

#include <sys/param.h>
#include "freertos/event_groups.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "addr_from_stdin.h"
#include "lwip/err.h"

#if defined(CONFIG_EXAMPLE_IPV4)
#define HOST_IP_ADDR CONFIG_EXAMPLE_IPV4_ADDR
#elif defined(CONFIG_EXAMPLE_IPV6)
#define HOST_IP_ADDR CONFIG_EXAMPLE_IPV6_ADDR
#else
#define HOST_IP_ADDR ""
#endif

#define PORT CONFIG_EXAMPLE_PORT

static const char *TAG = "example";
static const char *TAGSTATES = "STATES";
//static const char *payload = "Weights";

static const char *TAG2 = "Camera";
#define MOUNT_POINT "/sdcard"

#define BOARD_ESP32CAM_AITHINKER 1

// WROVER-KIT PIN Map
#ifdef BOARD_WROVER_KIT

#define SELECTED_BOARD 0
#define CAM_PIN_PWDN -1  //power down is not used
#define CAM_PIN_RESET -1 //software reset will be performed
#define CAM_PIN_XCLK 21
#define CAM_PIN_SIOD 26
#define CAM_PIN_SIOC 27

#define CAM_PIN_D7 35
#define CAM_PIN_D6 34
#define CAM_PIN_D5 39
#define CAM_PIN_D4 36
#define CAM_PIN_D3 19
#define CAM_PIN_D2 18
#define CAM_PIN_D1 5
#define CAM_PIN_D0 4
#define CAM_PIN_VSYNC 25
#define CAM_PIN_HREF 23
#define CAM_PIN_PCLK 22

#endif

// ESP32Cam (AiThinker) PIN Map
#ifdef BOARD_ESP32CAM_AITHINKER

#define SELECTED_BOARD 1

#define CAM_PIN_PWDN 32
#define CAM_PIN_RESET -1 //software reset will be performed
#define CAM_PIN_XCLK 0
#define CAM_PIN_SIOD 26
#define CAM_PIN_SIOC 27

#define CAM_PIN_D7 35
#define CAM_PIN_D6 34
#define CAM_PIN_D5 39
#define CAM_PIN_D4 36
#define CAM_PIN_D3 21
#define CAM_PIN_D2 19
#define CAM_PIN_D1 18
#define CAM_PIN_D0 5
#define CAM_PIN_VSYNC 25
#define CAM_PIN_HREF 23
#define CAM_PIN_PCLK 22

#endif

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
#define START_REQUEST_WEIGHTS	    0x06u
#define REQ_LAST_CHUNK			    0x07u

#define CAM_WIDTH   32
#define CAM_HEIGHT  32


#define WEIGHTS_SIZE            157328
#define WEIGHTS_CHUNKS          1024
#define WEIGHTS_LAST_CHUNK      (WEIGHTS_SIZE - (WEIGHTS_SIZE / WEIGHTS_CHUNKS)*WEIGHTS_CHUNKS)

// define socket requests
#define SOCK_REQ_DEFAULT            0x00u
#define SOCK_REQ_WEIGHTS            0x01u
#define SOCK_REQ_STATES             0x02u
#define SOCK_REQ_START_WEIGHTS      0x03u
#define SOCK_REQ_LAST_CHUNK         0x04u


#define SOCK_RESP_OK        0x01u
#define SOCK_RESP_NOK       0x00u


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
typedef struct{
    uint32_t lightsensorP;
	uint32_t usLeftP;
	uint32_t usRightP;
	uint32_t laserP;
	/* Performed previous action*/
	float leftM;
	float rightM;
    float reward;
    uint32_t lightsensorC;
	uint32_t usLeftC;
	uint32_t usRightC;
	uint32_t laserC;
    uint8_t imgp[CAM_HEIGHT*CAM_WIDTH];
    uint8_t imgc[CAM_HEIGHT*CAM_WIDTH];
}allStates;
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

int mySend(int socket, const void *buffer, size_t length, int flags)
{
    int totalSent = 0;
    int sent;
    while(totalSent < length)
    {
        sent = send(socket, buffer + totalSent, length - totalSent, flags);
        if(sent == -1)
        {
            ESP_LOGE(TAGSTATES, "Error sending data");
            return -1;
        }
        totalSent += sent;
    }
    return totalSent;
}


//Called after a transaction is queued and ready for pickup by master. We use this to set the handshake line high.
void my_post_setup_cb(spi_slave_transaction_t *trans) {
    WRITE_PERI_REG(GPIO_OUT_W1TS_REG, (1<<GPIO_HANDSHAKE));
}

//Called after transaction is sent/received. We use this to set the handshake line low.
void my_post_trans_cb(spi_slave_transaction_t *trans) {
    WRITE_PERI_REG(GPIO_OUT_W1TC_REG, (1<<GPIO_HANDSHAKE));
}

uint8_t* camera_frame;
uint8_t* camera_frame_prev;




static camera_config_t camera_config = {
    .pin_pwdn = CAM_PIN_PWDN,
    .pin_reset = CAM_PIN_RESET,
    .pin_xclk = CAM_PIN_XCLK,
    .pin_sscb_sda = CAM_PIN_SIOD,
    .pin_sscb_scl = CAM_PIN_SIOC,

    .pin_d7 = CAM_PIN_D7,
    .pin_d6 = CAM_PIN_D6,
    .pin_d5 = CAM_PIN_D5,
    .pin_d4 = CAM_PIN_D4,
    .pin_d3 = CAM_PIN_D3,
    .pin_d2 = CAM_PIN_D2,
    .pin_d1 = CAM_PIN_D1,
    .pin_d0 = CAM_PIN_D0,
    .pin_vsync = CAM_PIN_VSYNC,
    .pin_href = CAM_PIN_HREF,
    .pin_pclk = CAM_PIN_PCLK,

    //XCLK 20kHz or 10kHz for OV2640 double FPS (Experimental)
    .xclk_freq_hz = 20000000,
    .ledc_timer = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,

    .pixel_format = PIXFORMAT_GRAYSCALE, //YUV422,GRAYSCALE,RGB565,JPEG
    .frame_size = FRAMESIZE_UXGA,    //32x32-UXGA Do not use sizes above QVGA when not JPEG

    .jpeg_quality = 30, //0-63 lower number means higher quality
    .fb_count = 1       //if more than one, i2s runs in continuous mode. Use only with JPEG
};


static esp_err_t init_camera()
{

    if(1 == SELECTED_BOARD)
    {
        ESP_LOGE(TAG2, "ESP32CAM PIN Map selected");
    }

    //initialize the camera
    esp_err_t err = esp_camera_init(&camera_config);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG2, "Camera Init Failed");
        return err;
    }
    ESP_LOGE(TAG2, "Camera Init Succeeded");
    return ESP_OK;
}

static void tcp_client_task(void *pvParameters)
{
    char host_ip[] = HOST_IP_ADDR;
    int addr_family = 0;
    int ip_protocol = 0;

    printf("App_main started");
    int n=0;
    esp_err_t ret;
    uint8_t trans_state = STATE_WAITING_FOR_REQUEST;
    uint8_t trans_nextstate = STATE_WAITING_FOR_REQUEST;
    uint8_t request = 0u;
    bool skip = false;
    //Configuration for the SPI bus
    spi_bus_config_t buscfg={
        .mosi_io_num=GPIO_MOSI,
        .miso_io_num=GPIO_MISO,
        .sclk_io_num=GPIO_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz=WEIGHTS_CHUNKS+4
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

    /*Define DMA buffers in DMA capable memory*/
    uint8_t *weights_dma = (uint8_t *)heap_caps_malloc(WEIGHTS_CHUNKS, MALLOC_CAP_DMA);
    //uint8_t *sensorStates_dma = (uint8_t *)heap_caps_malloc(11*sizeof(uint32_t),MALLOC_CAP_DMA);
    camera_frame = (uint8_t *)heap_caps_malloc(CAM_WIDTH*CAM_HEIGHT, MALLOC_CAP_DMA);
    camera_frame_prev = (uint8_t *)heap_caps_malloc(CAM_WIDTH*CAM_HEIGHT, MALLOC_CAP_DMA);
    union sensorStateCU dataC;
    union sensorStatePU dataP;
    //memcpy(weights_dma,weights,sizeof(weights));
    printf("Sizeu: %u\n", sizeof(uint32_t));
    printf("Sizes: %u\n", sizeof(sensorState));
    printf("Sizew: %u\n", sizeof(weights_dma));
    /*for (int i =0; i<WEIGHTS_CHUNKS;i++)
    {
        weights_dma[i] = 0xAA;
    }*/
    /*for (int i =0; i<CAM_WIDTH*CAM_HEIGHT;i++)
    {
        camera_frame[i] = 0xBD;
        camera_frame_prev[i] = 0x11;
    }
    camera_frame[0] = 0xEA;
    camera_frame[CAM_WIDTH*CAM_HEIGHT-1] = 0xDB;*/
    //weights_dma[1] = 0xEE;
    //weights_dma[2500] = 0xCE;
    //weights_dma[4847] = 0xED;
    uint8_t req_sock = SOCK_REQ_DEFAULT;

    camera_fb_t *pic = NULL;
    allStates statesBuf;
    //memset(pic->buf,0x00,pic->len);
    int w_cnt = 0;
    while(1)
    {
        #if defined(CONFIG_EXAMPLE_IPV4)
            struct sockaddr_in dest_addr;
            dest_addr.sin_addr.s_addr = inet_addr(host_ip);
            dest_addr.sin_family = AF_INET;
            dest_addr.sin_port = htons(PORT);
            addr_family = AF_INET;
            ip_protocol = IPPROTO_IP;
        #elif defined(CONFIG_EXAMPLE_IPV6)
            struct sockaddr_in6 dest_addr = { 0 };
            inet6_aton(host_ip, &dest_addr.sin6_addr);
            dest_addr.sin6_family = AF_INET6;
            dest_addr.sin6_port = htons(PORT);
            dest_addr.sin6_scope_id = esp_netif_get_netif_impl_index(EXAMPLE_INTERFACE);
            addr_family = AF_INET6;
            ip_protocol = IPPROTO_IPV6;
        #elif defined(CONFIG_EXAMPLE_SOCKET_IP_INPUT_STDIN)
            struct sockaddr_storage dest_addr = { 0 };
            ESP_ERROR_CHECK(get_addr_from_stdin(PORT, SOCK_STREAM, &ip_protocol, &addr_family, &dest_addr));
        #endif
            
        int sock =  socket(addr_family, SOCK_STREAM, ip_protocol);
        if (sock < 0)
        {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            break;
        }
        
        ESP_LOGI(TAG, "Socket created, connecting to %s:%d", host_ip, PORT);

        int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr_in));
        
        if (err != 0)
        {
            ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
            break;
        }

        ESP_LOGI(TAG, "Successfully connected");
        w_cnt = 0;
        while (1)
        {


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
                printf("REQ_PING \n");
                t.length=8;
                sendbuf[0] = RESP_OK;
                t.tx_buffer = sendbuf;
                t.rx_buffer = NULL;
                trans_nextstate = STATE_WAITING_FOR_REQUEST;
                break;
            case START_REQUEST_WEIGHTS:
                printf("START_REQUEST_WEIGHTS\n");
                w_cnt = 0;
                req_sock = SOCK_REQ_START_WEIGHTS;
                mySend(sock, &req_sock, 1u, 0);
                sendbuf[0] = RESP_OK;
                t.tx_buffer = sendbuf;
                t.rx_buffer = NULL;
                trans_nextstate = STATE_WAITING_FOR_REQUEST;
                skip = true;
                break;
            case REQ_WEIGHTS:
                printf("REQ_WEIGHTS\n");
                
                t.tx_buffer = weights_dma;
                t.rx_buffer = NULL;
                req_sock = SOCK_REQ_WEIGHTS;
                printf("Receiving weights from PC\n");
                printf("Iterator: %d\n",w_cnt);
                mySend(sock, &req_sock, 1u, 0);
                recv(sock, weights_dma, WEIGHTS_CHUNKS, MSG_WAITALL);
                printf("Datafirst: %d\n",weights_dma[0]);
                
                t.length= 8 * WEIGHTS_CHUNKS;
                trans_nextstate = STATE_WAITING_FOR_REQUEST;
                w_cnt++;
                break;
            case REQ_LAST_CHUNK:
                req_sock = SOCK_REQ_LAST_CHUNK;
                t.tx_buffer = weights_dma;
                t.rx_buffer = NULL;
                printf("Receiving remaining weights: %d\n",WEIGHTS_SIZE - w_cnt * WEIGHTS_CHUNKS);
                mySend(sock, &req_sock, 1u, 0);
                recv(sock, weights_dma , WEIGHTS_SIZE - (WEIGHTS_SIZE / WEIGHTS_CHUNKS)*WEIGHTS_CHUNKS, MSG_WAITALL);
               
                t.length= 8 * (WEIGHTS_SIZE - (WEIGHTS_SIZE / WEIGHTS_CHUNKS)*WEIGHTS_CHUNKS);
                trans_nextstate = STATE_WAITING_FOR_REQUEST;
                w_cnt = 0;
                break;
            case REQ_SEND_STATE_1:
                printf("REQ_SEND_STATE_1\n");
                t.length = 8 * 7*sizeof(uint32_t);
                //memset(sensorStates_dma, 0xA5, 11*sizeof(uint32_t));
                t.tx_buffer=NULL;
                t.rx_buffer=recvbuf;
                dataP.buf = recvbuf;
                memset(recvbuf, 0xA5, 129);
                trans_nextstate = STATE_WAITING_FOR_REQUEST;
                break;
            case REQ_SEND_STATE_2:
                printf("REQ_SEND_STATE_2\n");
                t.length = 8 * 4*sizeof(uint32_t);
                //memset(sensorStates_dma, 0xA5, 11*sizeof(uint32_t));
                t.tx_buffer=NULL;
                t.rx_buffer=recvbuf;
                dataC.buf = recvbuf;
                memset(recvbuf, 0xA5, 129);
                
                
                // send picture too.
                trans_nextstate = STATE_WAITING_FOR_REQUEST;
                break;
            case REQ_CAMERA_PIC:
                printf("REQ_CAMERA_PIC\n");
                ESP_LOGI(TAG, "Taking picture...");
                pic = esp_camera_fb_get();
                //memset(pic->buf,0x43u,pic->len);
                //copy current image to the previous since it corresponds to the previous state by now
                //memcpy(camera_frame,pic->buf,CAM_WIDTH*CAM_HEIGHT);
                //memcpy(statesBuf.imgp,camera_frame_prev,CAM_WIDTH*CAM_HEIGHT);
                //memcpy(camera_frame_prev,camera_frame,CAM_WIDTH*CAM_HEIGHT);
                //memcpy(statesBuf.imgc,camera_frame,CAM_WIDTH*CAM_HEIGHT);
                
                
                for(int i = 0; i < pic->len; i++)
                {
                    camera_frame[i] = pic->buf[i];
                }
                for(int i = 0; i < pic->len; i++)
                {
                    statesBuf.imgp[i] = camera_frame_prev[i];
                }
                for(int i = 0; i < pic->len; i++)
                {
                    camera_frame_prev[i] = camera_frame[i];
                }
                for(int i = 0; i < pic->len; i++)
                {
                    statesBuf.imgc[i] = camera_frame[i];
                }
                ESP_LOGI(TAG, "For cycle finished...");
                /*
                err = send(sock, camera_frame, 1024u, 0);
                if (err < 0) 
                {
                    ESP_LOGE(TAG, "Error occurred during sending picture: errno %d", errno);
                    break;
                }

                if(!pic)
                {
                    ESP_LOGE(TAG, "Camera Capture Failed");
                }

                int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
                // Error occurred during receiving
                if (len < 0)
                {
                    ESP_LOGE(TAG, "recv failed: errno %d", errno);
                    break;
                }
                // Data received
                else {
                    rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
                    ESP_LOGI(TAG, "Received %d bytes from %s:", len, host_ip);
                    ESP_LOGI(TAG, "%s", rx_buffer);
                }*/
                /* Remember to use Semaphore here when integrating!!!*/
                //if(camera_frame[0] == 0xEA) printf("Equals");
                t.length = 8 * CAM_WIDTH*CAM_HEIGHT;
                t.tx_buffer = camera_frame;
                t.rx_buffer = NULL;
                trans_nextstate = STATE_WAITING_FOR_REQUEST;
                break;
            default:
                // useless data received presumably, so go back to request state
                printf("DEFAULT REQUEST handler, skipping\n");
                skip = true;
                t.length=8;
                sendbuf[0] = RESP_NOK;
                t.tx_buffer=sendbuf;
                t.rx_buffer=NULL;
                trans_nextstate = STATE_WAITING_FOR_REQUEST;
                break;
            }
            
            
            
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
                    
                    if((request == REQ_SEND_STATE_1) && (trans_state == STATE_SENDING_RESPONSE))
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
                        printf("\n SENDING SOCK_REQ_STATES TO PC VIA SOCKET\n");
                        req_sock = SOCK_REQ_STATES;
                        err = mySend(sock, &req_sock, 1u, 0);
                        if (err < 0) 
                        {
                            ESP_LOGE(TAG, "Error occurred during sending request header: errno %d", errno);
                            break;
                        }
                        statesBuf.laserP = dataP.statesP->laserP;
                        statesBuf.leftM = dataP.statesP->leftM;
                        statesBuf.lightsensorP = dataP.statesP->lightsensorP;
                        statesBuf.reward = dataP.statesP->reward;
                        statesBuf.rightM = dataP.statesP->rightM;
                        statesBuf.usLeftP = dataP.statesP->usLeftP;
                        statesBuf.usRightP = dataP.statesP->usRightP;
                        err = mySend(sock, dataP.buf, 28u, 0);
                        if (err < 0) 
                        {
                            ESP_LOGE(TAG, "Error occurred during sending state2: errno %d", errno);
                            break;
                        }

                    }
                    if(request == REQ_SEND_STATE_2 && trans_state == STATE_SENDING_RESPONSE)
                    {
                        
                        printf("Received: %s\n", recvbuf); 
                        printf("\n-----------STATES_CURR---------\n");
                        
                        printf("lightsensor value Current: %u\n", dataC.statesC->lightsensorC);
                        printf("usLeft value Current: %u\n", dataC.statesC->usLeftC);
                        printf("usRight value Current: %u\n", dataC.statesC->usRightC);
                        printf("laser value Current: %u\n", dataC.statesC->laserC);
                        printf("\n SENDING SENSORSTATES2 TO PC VIA SOCKET\n");
                        
                        
                        statesBuf.laserC = dataC.statesC->laserC;
                        statesBuf.lightsensorC = dataC.statesC->lightsensorC;
                        statesBuf.usLeftC = dataC.statesC->usLeftC;
                        statesBuf.usRightC = dataC.statesC->usRightC;
                        err = mySend(sock, dataC.buf, 16u, 0);
                        if (err < 0) 
                        {
                            ESP_LOGE(TAG, "Error occurred during sending state2: errno %d", errno);
                            break;
                        }
                        printf("\n SENDING camera_frame_prev TO PC VIA SOCKET\n");
                        vTaskDelay(2);
                        err = mySend(sock, camera_frame_prev, 1024u, 0);
                        if (err < 0) 
                        {
                            ESP_LOGE(TAG, "Error occurred during sending state2: errno %d", errno);
                            break;
                        }
                        printf("\n SENDING camera_frame TO PC VIA SOCKET\n");
                        vTaskDelay(2);
                        err = mySend(sock, camera_frame, 1024u, 0);
                        if (err < 0) 
                        {
                            ESP_LOGE(TAG, "Error occurred during sending state2: errno %d", errno);
                            break;
                        }
                        vTaskDelay(2);
                    }
                }
                
                
            } 
            if(t.tx_buffer != NULL) printf("Transmitted: %s\n", sendbuf);
            
        }
        
        //Update the state
        trans_state = trans_nextstate;
        n++;

        }

        if (sock != -1) 
        {
            ESP_LOGE(TAG, "Shutting down socket and restarting...");
            shutdown(sock, 0);
            close(sock);
        }
    }

         vTaskDelete(NULL);
}



void app_main(void)
{

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());

    init_camera();
    /*init_sdcard();*/

    xTaskCreate(tcp_client_task, "tcp_client", 4096, NULL, 5, NULL);
}
