#include "stdint.h"
#include "stm32f7xx_hal.h"
#include "string.h"

UART_HandleTypeDef huart3;

void PCsend(char* sendData);
