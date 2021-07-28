#include "main.h"

#define TX_PLOAD_WIDTH 1
#define TX_ADR_WIDTH 3

/*
//for Tx device 
uint8_t Tx_data[TX_PLOAD_WIDTH];
int main (void){
	GPIO();
	RCC_config();
	ADC1_config();
	CE_OFF();
	CS_OFF();
	SPI2_Init();
	NRF24_Tx_ini();
	//NRF24_Rx_ini();
	
	
	while (1)
	{
		Tx_data[0]= ADC1->DR;
		NRF24_Transmit(Tx_data);				
	}	
}	
*/


/*
//for Rx device
uint8_t Rx_data[TX_PLOAD_WIDTH];
int main (void){
	
	GPIO();
	RCC_config();
	CE_OFF();
	CS_OFF();
	SPI2_Init();
	NRF24_Rx_ini();
	while (1)
	{
		NRF24_Receive(Rx_data);		
	}
}

*/


uint8_t dt[5];
uint8_t Rx_data[TX_PLOAD_WIDTH];
int main (void){
	
	GPIO();
	RCC_config();
	CE_OFF();
	CS_OFF();
	SPI2_Init();
	NRF24_Rx_ini();
	while (1)
	{
		dt[1]=NRF24_ReadReg(CONFIG);
		dt[2]=NRF24_ReadReg(EN_AA);
		dt[3]=NRF24_ReadReg(STATUS);
		dt[4]=NRF24_ReadReg(EN_RXADDR);
	}
}


