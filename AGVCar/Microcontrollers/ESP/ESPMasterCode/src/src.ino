
#include "esp_timer.h"
#include "img_converters.h"
#include "Arduino.h"
#include "soc/soc.h"           // Disable brownour problems
#include "soc/rtc_cntl_reg.h"  // Disable brownour problems
#include "driver/rtc_io.h"

// Enables the use of a config variable
#include "camera_config.h"
// Contains wifi and socekt configs
#include "wifi_config.h"
#include "web_serial.h"
// Protocol definitions
#include "protocol_definitions.h"

#include "socket_communication.h"

#include "spi_config.h"


#define WEIGHTS_SIZE        157328
#define CHUNK_SIZE          1024
#define LAST_CHUNK_SIZE     (WEIGHTS_SIZE - (WEIGHTS_SIZE / CHUNK_SIZE)*CHUNK_SIZE)
#define NUMBER_OF_CHUNKS    (WEIGHTS_SIZE / CHUNK_SIZE)

// Webserver
AsyncWebServer server(80);

// Wifi socket
WiFiClient client;

// Nucleo interface
SPIClass * nucleo = NULL; // SPI class interface, cannot send more than one byte


// State of ESP
uint8_t state = STATE_LISTEN;

void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);
  esp_err_t err;
  pinMode(GPIO_SS, OUTPUT); //HSPI SS
  digitalWrite(GPIO_SS, HIGH);
  
  //Nucleo setup with SPI Classs
  /*nucleo = new SPIClass(HSPI);
  nucleo->begin(GPIO_SCLK, GPIO_MISO, GPIO_MOSI, GPIO_SS);
  pinMode(GPIO_SS, OUTPUT); //HSPI SS
  pinMode(GPIO_HANDSHAKE, INPUT); //HSPI HANDSHAKE
  digitalWrite(GPIO_SS, HIGH); // Write HIGH to signal no incoming transaction
  digitalWrite(GPIO_HANDSHAKE, LOW);*/
  
  // Final camera configs
  // OV2640 camera module
  err = initCamera();
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    ESP.restart();
  }
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  // Print ESP32 Local IP Address
  Serial.print("IP Address: http://");
  Serial.println(WiFi.localIP());
  // WebSerial is accessible at "<IP Address>/webserial" in browser
  WebSerial.begin(&server);
  WebSerial.msgCallback(recvMsg);
  server.begin();
  
  // Turn-off the 'brownout detector'
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  
  // Conncect to socket
  if (!client.connect(host, port)) {
      //WebSerial.println("Connection to host failed");
      delay(1000);
      return;
  }
  //WebSerial.println("Connected to server successful!");
   digitalWrite(GPIO_SS, LOW);
   delay(5000);

   
  err = fInitializeSPI_Channel();
  WebSerial.println(err);
  if (err != ESP_OK) {
    WebSerial.println("Spi channel init failes, error code: ");
    WebSerial.println(err);
    ESP.restart();
  }
    
  err = fInitializeSPI_Devices(spi);
  WebSerial.println(err);
  if (err != ESP_OK) {
    WebSerial.print("Spi device init failes, error code: ");
    WebSerial.println(err);
    ESP.restart();
  }
}

void loop() {
    while (client.connected()) {
      uint8_t action = getRequestFromSocket(client, state);
      //WebSerial.println(action);
      handleAction(action);
      delay(1);
    }
    client.stop();
    //WebSerial.println("Client disconnected");
}

void handleAction(uint8_t action)
{
  switch(action){
    case SOCK_REQ_STOP:
        sendRequestToSlave(spi, action);
        break;
    case SOCK_REQ_LINE_FOLLOWING:
        sendRequestToSlave(spi, action);
        break;
    case SOCK_REQ_SEND_IMAGE:
        sendImageToSocket();
        break;
    case SOCK_REQ_WAIT_FOR_WEIGHTS:
        sendRequestToSlave(spi, action);
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



void getWeightsFromSocket()
{/*
  Serial.println("Requesting weights");
  uint8_t buffer[CHUNK_SIZE];
  for(int i = 0; i<NUMBER_OF_CHUNKS; i++)
  {
     
      while(!client.available()){}
      char c = client.read(buffer, CHUNK_SIZE);
      for(int j = 0; j<CHUNK_SIZE; j++)
      {
        while(digitalRead(GPIO_HANDSHAKE)==HIGH){}
        nucleo->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
        digitalWrite(GPIO_SS, LOW);
        delay(2); // We delay so we can make sure the slave is listening
        nucleo->transfer(buffer[j]);
        digitalWrite(GPIO_SS, HIGH);
        nucleo->endTransaction();
      }
  }
  Serial.println("Requesting lst chunk of weights"); 
  while(!client.available()){}
  char c = client.read(buffer, LAST_CHUNK_SIZE);
  for(int j = 0; j<LAST_CHUNK_SIZE; j++)
  {
    while(digitalRead(GPIO_HANDSHAKE)==HIGH){}
    nucleo->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
    digitalWrite(GPIO_SS, LOW);
    delay(2); // We delay so we can make sure the slave is listening
    nucleo->transfer(buffer[j]);
    digitalWrite(GPIO_SS, HIGH);
    nucleo->endTransaction();
  }*/
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
