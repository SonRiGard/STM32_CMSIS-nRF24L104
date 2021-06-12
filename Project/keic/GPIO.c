#include "GPIO.h"

void GPIO (void){
	//gpioc 13 out put

	
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	GPIOC->BSRR |= GPIO_BSRR_BS13 ;
	GPIOC->CRH &= ~GPIO_CRH_CNF13;//general purpose output push-pull
	
	GPIOC->CRH &= ~GPIO_CRH_MODE13;
		// GPIO PA8 out put dung PWM
	
	// MODE 10: Alternate function output Push-pull
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	GPIOA->CRH &= ~GPIO_CRH_CNF8;
	GPIOA->CRH |= GPIO_CRH_CNF8_1;// Alternate function output Push-pull
	GPIOA->CRH |= GPIO_CRH_MODE8_1;//output-2Hz
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
	
	//ADC1 -> PA0 (CH1)
		//Analog for PA0
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	
	GPIOA->CRL &= ~GPIO_CRL_CNF0;//ANALOG
	GPIOA->CRL &= ~GPIO_CRL_MODE0;//INPUT MODE
	
}