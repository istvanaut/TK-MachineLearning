/* BSD Socket API Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

// xSemaphoreGive
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
#include "driver/spi_master.h"
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
#include "freertos/sm.h"
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

#define MOUNT_POINT "/sdcard"


// WROVER-KIT PIN Map
#include "headers/board_wrover_kit.h"

// ESP32Cam (AiThinker) PIN Map
#include "headers/borard_esp32cam_aithinker.h"

/*
SPI receiver
*/
#include "headers/spi_reciever.h"

// define response constants
#define RESP_OK    0x01u
#define RESP_NOK   0x00u

// define 

#define CAM_WIDTH   96
#define CAM_HEIGHT  96


#define WEIGHTS_SIZE            157328
#define WEIGHTS_CHUNKS          1024
#define WEIGHTS_LAST_CHUNK      (WEIGHTS_SIZE - (WEIGHTS_SIZE / WEIGHTS_CHUNKS)*WEIGHTS_CHUNKS)

// define socket requests
#define SOCK_REQ_STOP               0x00u
#define SOCK_REQ_LINE_FOLLOWING     0x01u
#define SOCK_REQ_SEND_IMAGE         0x02u
#define SOCK_REQ_WAIT_FOR_WEIGHTS   0x03u
#define SOCK_REQ_NETWORK            0x04u
#define SOCK_REQ_CONTINUE           0x05u

// define state
#define STATE_LISTEN                0x00u
#define STATE_START_WEIGHTS         0x01u
#define STATE_SEND_CHUNK            0x02u
#define STATE_SEND_LAST_CHUNK       0x03u


#define SOCK_RESP_OK        0x01u
#define SOCK_RESP_NOK       0x00u

#include "headers/sensor_states.h"


static const char *TAG = "example";
static const char *TAGSTATES = "STATES";
//static const char *payload = "Weights";

static const char *TAG2 = "Camera";
uint8_t recieved_state[1];
uint8_t* weights_dma;
uint8_t* camera_frame;
uint8_t* camera_frame_prev;


spi_device_handle_t spi;

int sendToSocket(int socket, const void *buffer, size_t length, int flags)
{
    int totalSent = 0;
    while(totalSent < length)
    {
        int sent = send(socket, buffer + totalSent, length - totalSent, flags);
        if(sent == -1)
        {
            ESP_LOGE(TAGSTATES, "Error sending data");
            return -1;
        }
        totalSent += sent;
    }
    return totalSent;
}

static esp_err_t initCamera()
{
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

spi_bus_config_t buscfg={
    .sclk_io_num = GPIO_SCLK, // CLK
    .mosi_io_num = GPIO_MOSI, // MOSI
    .miso_io_num = GPIO_MISO, // MISO
    .quadwp_io_num = -1, // Not used
    .quadhd_io_num = -1, // Not used
    .max_transfer_sz = 1024,
};

spi_device_interface_config_t devcfg={
    .address_bits     = 0,
    .command_bits     = 0,
    .dummy_bits       = 0,
    .mode             = 1,      // for DMA, only 1 or 3 is available
    .duty_cycle_pos   = 0,
    .cs_ena_posttrans = 0,
    .cs_ena_pretrans  = 0,
    .clock_speed_hz   = 10*1000*1000, 
    .spics_io_num     = GPIO_CS,
    .flags            = 0,
    .queue_size       = 1,
    .pre_cb           = NULL,
    .post_cb          = NULL,   // We want to be able to queue 7 transactions at a time
};

static void configureSPI(){
    esp_err_t ret;
    //Configuration for the SPI bus
    ret = spi_bus_initialize(HSPI_HOST, &buscfg, DMA_CHAN);
    ESP_ERROR_CHECK(ret);

    //Attach the NUCLEO to the SPI bus
    ret=spi_bus_add_device(HSPI_HOST, &devcfg, &spi);
    ESP_ERROR_CHECK(ret);
}

static int connectToSocket(){
    char host_ip[] = HOST_IP_ADDR;
    int addr_family = 0;
    int ip_protocol = 0;
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
        return -1;
    }
    
    ESP_LOGI(TAG, "Socket created, connecting to %s:%d", host_ip, PORT);

    int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr_in));
    
    if (err != 0)
    {
        ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
        return -1;
    }

    ESP_LOGI(TAG, "Successfully connected");
    return sock;
}

static void getRequestFromSokcet(int sock, uint8_t state){
    uint8_t req_sock;
    ESP_LOGI(TAG, "Requesting state...");
    req_sock = state;
    sendToSocket(sock, &req_sock, 1u, 0);
    recv(sock, recieved_state, 1, MSG_WAITALL);
    sendToSocket(sock, &recieved_state, 1u, 0);
    ESP_LOGI(TAG, "Requesting chunk: %d", recieved_state[0]);
}

static void sendRequestToSlave(uint8_t request){
    esp_err_t err;
    uint8_t txData[1] = { };
    txData[0] = request; // command bits;
    uint8_t rxData[1] = { };
    spi_transaction_t trans_desc;
    memset(&trans_desc, 0, sizeof(trans_desc));
    trans_desc.addr =  0;
    trans_desc.cmd = 0;
    trans_desc.flags = 0 ;
    trans_desc.length = (8*1); // total data bits
    trans_desc.tx_buffer = txData;
   
    err = spi_device_transmit(spi, &trans_desc);
    ESP_LOGE(TAG, "Transaction error: %d", err);
    ESP_ERROR_CHECK(err);
}

static void getWeightsFromSocket(spi_device_handle_t spi, int sock){
    int i;
    uint8_t req_sock;
    static spi_transaction_t trans[(WEIGHTS_SIZE / WEIGHTS_CHUNKS)+1];
    esp_err_t ret;
    // Signal to socket to prepare weights
    ESP_LOGI(TAG, "Requesting weights...");
    req_sock = STATE_START_WEIGHTS;
    sendToSocket(sock, &req_sock, 1u, 0);
    // Request weights
    for(i =0; i<(WEIGHTS_SIZE / WEIGHTS_CHUNKS); ++i){
        ESP_LOGI(TAG, "Requesting chunk: %d", i);
        memset(&trans[i], 0, sizeof(spi_transaction_t));
        req_sock = STATE_SEND_CHUNK;
        sendToSocket(sock, &req_sock, 1u, 0);
        recv(sock, weights_dma, WEIGHTS_CHUNKS, MSG_WAITALL);
        trans[i].length=8*WEIGHTS_CHUNKS;
        trans[i].tx_buffer = weights_dma;
        trans[i].rx_buffer = NULL;
        ret=spi_device_queue_trans(spi, &trans[i], portMAX_DELAY);
        assert(ret==ESP_OK);
    }
    ESP_LOGI(TAG, "Requesting last weights...");
    memset(&trans[i], 0, sizeof(spi_transaction_t));
    req_sock = STATE_SEND_LAST_CHUNK;
    sendToSocket(sock, &req_sock, 1u, 0);
    recv(sock, weights_dma, WEIGHTS_LAST_CHUNK, MSG_WAITALL);
    trans[i].length=8*WEIGHTS_CHUNKS;
    trans[i].tx_buffer = weights_dma;
    trans[i].rx_buffer = NULL;
    ret=spi_device_queue_trans(spi, &trans[i], portMAX_DELAY);
    assert(ret==ESP_OK);
}

static void sendImageToSocket(int sock, camera_fb_t* image){
    printf("\n SENDING camera_frame TO PC VIA SOCKET\n");
    ESP_LOGI(TAG2, "image len: %d", image->len);
    vTaskDelay(2);
    int err = sendToSocket(sock, image->buf, image->len, 0); 
    if (err < 0) 
    {
        ESP_LOGE(TAG, "Error occurred during sending image: errno %d", err);
    }
    vTaskDelay(2);
    esp_camera_fb_return(image);
}

static void handleRequest(spi_device_handle_t spi, int sock){
    switch(recieved_state[0]){
        case SOCK_REQ_STOP:
            sendRequestToSlave(recieved_state[0]);
            break;
        case SOCK_REQ_LINE_FOLLOWING:
            sendRequestToSlave(recieved_state[0]);
            break;
        case SOCK_REQ_SEND_IMAGE:;
            camera_fb_t *image = NULL;
            image = esp_camera_fb_get();
            sendImageToSocket(sock, image);
            break;
        case SOCK_REQ_WAIT_FOR_WEIGHTS:
            getWeightsFromSocket(spi, sock);
            break;
        case SOCK_REQ_NETWORK:
            break;
        case SOCK_REQ_CONTINUE:
            break;
    }
}

static void tcpClientTask(void *pvParameters)
{
    printf("App_main started");
    configureSPI();
    camera_frame = (uint8_t *)heap_caps_malloc(CAM_WIDTH*CAM_HEIGHT, MALLOC_CAP_DMA);
    uint8_t state = STATE_LISTEN;
    printf("Initialized\n");
    while(true)
    {
        int sock=connectToSocket();
        if(sock==-1)
            break;
        while (true)
        {
            getRequestFromSokcet(sock, state);
            handleRequest(NULL, sock);
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

    initCamera();
    /*init_sdcard();*/

    xTaskCreate(tcpClientTask, "tcp_client", 4096, NULL, 5, NULL);
}
