
#include "WiFi.h"
#include "esp_camera.h"
#include "esp_timer.h"
#include "img_converters.h"
#include "Arduino.h"
#include "soc/soc.h"           // Disable brownour problems
#include "soc/rtc_cntl_reg.h"  // Disable brownour problems
#include "driver/rtc_io.h"
#include <ESPAsyncWebServer.h>
#include <StringArray.h>
#include <SPIFFS.h>
#include <FS.h>
// Enables the use of a config variable
#include "camera_config.h"
// Contains wifi and socekt configs
#include "wifi_config.h"
// Html layout for webserver
#include "web_html.h"
// Protocol definitions
#include "protocol_definitions.h"

#include "socket_communication.h"

#include "spi_config.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

#define WEIGHTS_SIZE        157328
#define CHUNK_SIZE          1024
#define LAST_CHUNK_SIZE     (WEIGHTS_SIZE - (WEIGHTS_SIZE / CHUNK_SIZE)*CHUNK_SIZE)
#define NUMBER_OF_CHUNKS    (WEIGHTS_SIZE / CHUNK_SIZE)

WiFiClient client;
uint8_t state = 0;

SPIClass * nucleo = NULL;
  //spi_device_handle_t spi;
  
void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);

  nucleo = new SPIClass(HSPI);
  nucleo->begin(GPIO_SCLK, GPIO_MISO, GPIO_MOSI, GPIO_SS);
  pinMode(GPIO_SS, OUTPUT); //HSPI SS
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    ESP.restart();
  }
  else {
    delay(500);
    Serial.println("SPIFFS mounted successfully");
  }

  // Print ESP32 Local IP Address
  Serial.print("IP Address: http://");
  Serial.println(WiFi.localIP());

  // Turn-off the 'brownout detector'
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);


  // Final camera configs
  // OV2640 camera module
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_GRAYSCALE;
  if (psramFound()) {
    config.frame_size = FRAMESIZE_96X96;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_96X96;
    config.jpeg_quality = 10;
    config.fb_count = 1;
  }
  // Camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    ESP.restart();
  }
  // Conncect to socket
  if (!client.connect(host, port)) {

      Serial.println("Connection to host failed");

      delay(1000);
      return;
  }

  Serial.println("Connected to server successful!");
 /*/ unsigned char hexBuffer[4];
  // Intial state
  state = STATE_LISTEN;
  spi_bus_config_t buscfg;
        buscfg.miso_io_num = GPIO_MISO;
        buscfg.mosi_io_num = GPIO_MOSI;
        buscfg.sclk_io_num = GPIO_SCLK;
        buscfg.quadwp_io_num = -1;
        buscfg.quadhd_io_num = -1;
        buscfg.max_transfer_sz = 8+4;
  //Initialize the SPI bus
  ret = spi_bus_initialize(HSPI_HOST, &buscfg, 2);
  memcpy((char*)hexBuffer,(char*)&ret,sizeof(int));
  size_t sent = client.write(hexBuffer, 4);
  ESP_ERROR_CHECK(ret);
  
  spi_device_interface_config_t devcfg;
        devcfg.clock_speed_hz=1*1000*1000;           //Clock out at 10 MHz
        devcfg.mode=0;                               //SPI mode 0
        devcfg.spics_io_num=GPIO_SS;               //CS pin
        devcfg.queue_size=1;                          //We want to be able to queue 7 transactions at a time
  
  ret=spi_bus_add_device(HSPI_HOST, &devcfg, &spi);
  client.write(hexBuffer, 4);
  ESP_ERROR_CHECK(ret);*/
  digitalWrite(GPIO_SS, HIGH);
}

void loop() {
    while (client.connected()) {
      Serial.println("Connected to server successful!");

      uint8_t action = getRequestFromSocket(client, state);
      handleAction(action);
      delay(1);
      
      delay(10);
    }
    client.stop();
    Serial.println("Client disconnected");
}

void handleAction(uint8_t action)
{
  switch(action){
    case SOCK_REQ_STOP:
        sendRequestToSlave(action);
        break;
    case SOCK_REQ_LINE_FOLLOWING:
        sendRequestToSlave(action);
        break;
    case SOCK_REQ_SEND_IMAGE:
        sendImageToSocket();
        break;
    case SOCK_REQ_WAIT_FOR_WEIGHTS:
        getWeightsFromSocket();
        break;
    case SOCK_REQ_NETWORK:
        break;
    case SOCK_REQ_CONTINUE:
        break;  
    default:
      break; 
  }  
}

void sendRequestToSlave(uint8_t request) {
  /*esp_err_t ret;
  static spi_transaction_t trans[1];
  trans[0].length=8;
  trans[0].flags=SPI_TRANS_USE_TXDATA;
  trans[0].tx_data[0]=request;
  ret=spi_device_queue_trans(spi, &trans[0], portMAX_DELAY);
  assert(ret==ESP_OK);*/
  nucleo->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
  digitalWrite(GPIO_SS, LOW);
  delay(400);
  uint8_t resp = nucleo->transfer(request);
  digitalWrite(GPIO_SS, HIGH);
  nucleo->endTransaction();
}

void getWeightsFromSocket()
{
  Serial.println("Requesting weights");
  uint8_t buffer[CHUNK_SIZE];
  for(int i = 0; i<NUMBER_OF_CHUNKS; i++)
  {
      while(!client.available()){}
      char c = client.read(buffer, CHUNK_SIZE);
      for (int i = 0; i < CHUNK_SIZE; i++) Serial.print(buffer[i], HEX);
      Serial.println();
      Serial.println(i);
      Serial.println(NUMBER_OF_CHUNKS);
  }
 
  Serial.println("Requesting lst chunk of weights"); 
  while(!client.available()){}
  char c = client.read(buffer, LAST_CHUNK_SIZE);
  for (int i = 0; i < LAST_CHUNK_SIZE; i++) Serial.print(buffer[i], HEX);
  Serial.println();
}

// Capture Photo and Save it to SPIFFS
void sendImageToSocket( void ) {
  camera_fb_t * fb = NULL; // pointer
 
  // Take a photo with the camera
  Serial.println("Taking a photo...");

  fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  const char *data = (const char *)fb->buf;
  size_t sent = client.write(data, fb->len);
  Serial.print("THe picture has been sent");

  esp_camera_fb_return(fb);
 
}
