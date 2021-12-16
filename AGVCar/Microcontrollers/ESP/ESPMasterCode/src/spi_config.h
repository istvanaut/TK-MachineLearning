#include <SPI.h>
#include "driver/spi_master.h"
#include "driver/gpio.h"

#define GPIO_MOSI   12
#define GPIO_MISO   13
#define GPIO_SCLK   15
#define GPIO_SS     GPIO_NUM_14
#define GPIO_HANDSHAKE     2
#define EnableDMA true

static const int SPI_Clk = 1000000; // 1 MHz
spi_bus_config_t bus_config = { };
spi_device_interface_config_t dev_config = { };  // initializes all field to 0
spi_device_handle_t spi; // ESP-IDF spi interface
  
esp_err_t fInitializeSPI_Channel()
{
  esp_err_t intError;

  bus_config.sclk_io_num = GPIO_SCLK; // CLK
  bus_config.mosi_io_num = GPIO_MOSI; // MOSI
  bus_config.miso_io_num = GPIO_MISO; // MISO
  bus_config.quadwp_io_num = -1; // Not used
  bus_config.quadhd_io_num = -1; // Not used
  bus_config.max_transfer_sz = 1024;
  intError = spi_bus_initialize( HSPI_HOST, &bus_config, EnableDMA) ;
  return intError;
}

esp_err_t fInitializeSPI_Devices(spi_device_handle_t h)
{
  esp_err_t intError;
  dev_config.address_bits     = 0;
  dev_config.command_bits     = 0;
  dev_config.dummy_bits       = 0;
  dev_config.mode             = 1;// for DMA, only 1 or 3 is available
  dev_config.duty_cycle_pos   = 0;
  dev_config.cs_ena_posttrans = 0;
  dev_config.cs_ena_pretrans  = 0;
  dev_config.clock_speed_hz   = SPI_Clk;
  dev_config.spics_io_num     = GPIO_SS;
  dev_config.flags            = 0;
  dev_config.queue_size       = 1;
  dev_config.pre_cb           = NULL;
  dev_config.post_cb          = NULL;
  intError = spi_bus_add_device(HSPI_HOST, &dev_config, &spi);
  return intError;
} 

void sendRequestToSlave(spi_device_handle_t h, uint8_t request) {
  esp_err_t err;
  uint8_t txData[1] = { };
  uint8_t rxData[1] = { };
  spi_transaction_t trans_desc;
  memset(&trans_desc, 0, sizeof(trans_desc));
  trans_desc.addr =  0;
  trans_desc.cmd = 0;
  trans_desc.flags = 0 ;
  trans_desc.length = (8*1); // total data bits
  trans_desc.tx_buffer = txData;
  txData[0] = request; // command bits
  err = spi_device_acquire_bus(spi, portMAX_DELAY);
  WebSerial.println(err);
  delay(5000);
  err = spi_device_polling_start( spi, &trans_desc, portMAX_DELAY);
  WebSerial.println(err);
  delay(5000);
  err = spi_device_polling_end(spi, portMAX_DELAY);
  WebSerial.println(err);
  delay(5000);
  spi_device_release_bus(spi);
  if (err != ESP_OK) {
    WebSerial.print("Transaction failed, error code: ");
    WebSerial.println(err);
    ESP.restart();
  }
  /*
  nucleo->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
  digitalWrite(GPIO_SS, LOW);
  delay(250); // We delay so we can make sure the slave is listening
  uint8_t resp = nucleo->transfer(request);
  digitalWrite(GPIO_SS, HIGH);
  nucleo->endTransaction();*/
}
