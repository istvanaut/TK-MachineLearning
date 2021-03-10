/*
 * lightSensor.c
 *
 *  Created on: Mar 10, 2021
 *      Author: Kristof
 */

#include "lightSensor.h"


void SPItransmit(uint8_t leds)
{
	// Check if the SPI is enabled
	  /*if((SPI2->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE)
	  {
	      // If disabled, I enable it
	      SET_BIT(SPI2->CR1, SPI_CR1_SPE);
	  }*/

	// Wait until SPI is ready
	while (!(SPI2->SR & SPI_SR_TXE));

	// Send bytes over the SPI
	HAL_SPI_Transmit(&hspi2,  leds, 1, 1000);
	HAL_GPIO_WritePin(GPIOB, LED_LE_Pin, GPIO_PIN_SET);
	for(int j=0; j<1000; j++);
	HAL_GPIO_WritePin(GPIOB, LED_LE_Pin, GPIO_PIN_RESET);

	// Wait until the transmission is complete
	while (SPI2->SR & SPI_SR_BSY);






	  /*// Disable SPI
	  CLEAR_BIT(SPI2->CR1, SPI_CR1_SPE);*/
}

