#include "TIM2.h"

void DelayMili(uint16_t Delay){
	TIM2_Denit();
	Delay = Delay << 1;
	RCC->APB1ENR &= ~RCC_APB1ENR_TIM2EN;
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

	TIM2->PSC = 35999; //f = 2kHz
	TIM2->ARR = Delay;

	TIM2->CR1 |= TIM_CR1_CEN;

	while(!(TIM2->SR & TIM_SR_UIF));
	TIM2->CR1 &= ~TIM_CR1_CEN;
}


void DelayMicro(uint16_t Delay){//1 tick = 1 us
	TIM2_Denit();

	RCC->APB1ENR &= ~RCC_APB1ENR_TIM2EN;
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

	TIM2->PSC = 71; //f = 1MHz
	TIM2->ARR = Delay;

	TIM2->CR1 |= TIM_CR1_CEN;

	while(!(TIM2->SR & TIM_SR_UIF));
	TIM2->CR1 &= ~TIM_CR1_CEN;
}

void TIM2_Denit(void)
{
	TIM2->CR1 = 0;
	TIM2->CR2 = 0;
	TIM2->SR = 0;
	TIM2->DIER = 0;
	TIM2->EGR = 0;
	TIM2->PSC = 0;
	TIM2->ARR = 0xFFF;
}

