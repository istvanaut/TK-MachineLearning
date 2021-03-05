#include "myCOM.h"


void PCsend(char* sendData){
	HAL_UART_Transmit(&huart3, (uint8_t*)sendData, strlen(sendData), 15000);
}
