/*
 * lightSensor.h
 *
 *  Created on: Mar 10, 2021
 *      Author: Kristof
 */

#include "main.h"

extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;

void SPItransmit(uint8_t leds);
