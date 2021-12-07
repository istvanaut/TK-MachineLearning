#include <SPI.h>
#define GPIO_MOSI   12
#define GPIO_MISO   13
#define GPIO_SCLK   15
#define GPIO_SS     14

#define GPIO_HANDSAKE     2

static const int spiClk = 1000000; // 1 MHz
