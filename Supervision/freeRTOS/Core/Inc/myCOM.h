/**
 * myCom.h
 * 		Author: Kolontari Peter
 * 	Created on: Mar 8, 2021
 * 	All Rights Reserved.
 */

#include "main.h"
#include "stdio.h"

void initMyCOM(UART_HandleTypeDef* PC_UART);

int __io_putchar(int ch);

int __io_getchar(void);

int _write(int file, char* data, int len);

int _read(int file, char *ptr, int len);
