/**
 * myCom.c
 * 		Author: Kolontari Peter
 * 	Created on: Mar 8, 2021
 * 	All Rights Reserved.
 */

#include "myCOM.h"

UART_HandleTypeDef* myHuart;

/**
 * @brief  Set which UART is communicating with PC to override printf
 * @param  PC_UART Pointer to a UART_HandleTypeDef structure that communicate with PC
 */
void initMyCOM(UART_HandleTypeDef* PC_UART){
	myHuart = PC_UART;
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stdin, NULL, _IONBF, 0);
}

int __io_putchar(int ch){
	HAL_UART_Transmit(myHuart, (uint8_t*)&ch, 1, 10);
	return ch;
}

int __io_getchar(void){
	uint8_t data;
	HAL_UART_Receive(myHuart, &data, 1, 0xffff);

	return data;
}

int _write(int file, char* data, int len){
	for(int i = 0; i < len; i++){
		__io_putchar(*data);
		if(*data == '\n') __io_putchar('\r');
		data++;
	}

	return len;
}

int _read(int file, char *ptr, int len){
	for(int i = 0; i < len; i++){
		*ptr = (char)__io_getchar();
		ptr++;
	}

	return len;
}

