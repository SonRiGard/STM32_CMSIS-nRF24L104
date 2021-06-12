#include "main.h"
#include "stdio.h"
int main (void){
	GPIO();
	RCC_config();
	ADC1_config();
	//PWM();
	all_motor_config();

	while (1){
	TIM4->CCR1 = (ADC1->DR)/2+2000  ;
	TIM4->CCR2 = (ADC1->DR)/2+2000  ;
	TIM4->CCR3 = (ADC1->DR)/2+2000  ;
	TIM4->CCR4 = (ADC1->DR)/2+2000  ;
	printf("so nay la %d",ADC1->DR);
}
}	

