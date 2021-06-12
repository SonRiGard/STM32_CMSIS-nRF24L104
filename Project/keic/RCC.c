#include "RCC.h"

void RCC_config (void){
  RCC->CR |= ((uint32_t)RCC_CR_HSEON);	 //HSE enable
	while(!(RCC->CR & RCC_CR_HSERDY));     //wait still HSE ready
	
	FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY; // Clock Flash memory
	
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1; // AHB = SYSCLK
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV2; //APB1 = AHB/2 = HCLK/2 = 72/2 = 36 MHz
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV1; //APB2 = AHB = HCLK = 72 MHz
	RCC->CFGR |= RCC_CFGR_MCO_SYSCLK; //System clock (SYSCLK) selected for Clock OUTPUT
	
	RCC->CFGR &= ~RCC_CFGR_PLLMULL;  //Clear PLLBIL bits
	RCC->CFGR &= ~RCC_CFGR_PLLSRC;   //Clear PLLSRC bit
	RCC->CFGR &= ~RCC_CFGR_PLLXTPRE; //Clear PLLXTPRE bit / HSE divider for PLL entry
	
	RCC->CFGR |= RCC_CFGR_PLLSRC; //HSE oscillator clock selected as PLL input clock
	RCC->CFGR &= ~RCC_CFGR_PLLXTPRE; //HSE clock not divided
	RCC->CFGR |= RCC_CFGR_PLLMULL9; // PLL x9: clock = 8 MHz * 9 = 72 MHz
	
	RCC->CR |= RCC_CR_PLLON; //PLL on
	while((RCC->CR & RCC_CR_PLLRDY)==0); // wait till PLL ready
	
	RCC->CFGR &= ~RCC_CFGR_SW; //clear SW bits
	RCC->CFGR |= RCC_CFGR_SW_PLL; //PLL selected as system clock
	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_1) {}; 			// wait till PLL is used	
}
