/*
 * lightSensor.h
 *
 *  Created on: Mar 10, 2021
 *      Author: Kristof
 */

#include "main.h"

extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;

void lightSensorCycle();
void SPItransmit_LED(uint8_t* leds, int ledType);
uint16_t SPIreceive_AD(int idx);
