#include "SPI.h"

void SPI2_Init (void){
//SPI GPIO CONFIG
//	PB12->NSS Hardware master/ NSS output enabled->Alternate function push-pull
//	PB13->SCK Master -> Alternate function push-pull
//	PB14->MISO Full duplex / master ->Input floating (/ Input pull-up)
//	PB15->MOSI Full duplex / master ->Alternate function push-pull
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
	
	GPIOB->CRH |= GPIO_CRH_CNF12_1;
	GPIOB->CRH &= ~GPIO_CRH_CNF12_0;
	GPIOB->CRH |= GPIO_CRH_MODE12;
	
	GPIOB->CRH |= GPIO_CRH_CNF13_1;
	GPIOB->CRH &= ~GPIO_CRH_CNF13_0;
	GPIOB->CRH |= GPIO_CRH_MODE13;
	
	GPIOB->CRH &= ~GPIO_CRH_CNF14_1;
	GPIOB->CRH |= GPIO_CRH_CNF14_0;
	GPIOB->CRH &= ~GPIO_CRH_MODE14;
	
	GPIOB->CRH |= GPIO_CRH_CNF15_1;
	GPIOB->CRH &= ~GPIO_CRH_CNF15_0;
	GPIOB->CRH |= GPIO_CRH_MODE15;
	
	SPI2_Denit();
	RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
}

void SPI2_Denit(void){
//	while (!(SPI1->SR & SPI_SR_TXE)){};
//	while (SPI1->SR & SPI_SR_BSY){};
	SPI2->CR1 = 0x0000;
	SPI2->CR2 = 0x0000;
	SPI2->DR = 0x0000;
}
