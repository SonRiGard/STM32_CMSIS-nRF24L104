#include "main.h"

#define TX_PLOAD_WIDTH 2
#define TX_ADR_WIDTH 3


//for Tx device 
//uint8_t Tx_data[TX_PLOAD_WIDTH];
//int main (void){
//	GPIO();
//	RCC_config();
//	ADC1_config();
//	CE_OFF();
//	CS_OFF();
//	SPI2_Init();
//	NRF24_Tx_ini();
//	//NRF24_Rx_ini();	 
//	CE_ON();
//	while (1)
//	{	
//		//Tx_data[0]= ADC1->DR;
//		Tx_data[0]=12;
//		Tx_data[1]=13;
//		
//		NRF24_Transmit(Tx_data);
//		DelayMili(100);
//	}	
//}


//for Rx device
uint8_t Rx_data[TX_PLOAD_WIDTH];
int main (void){
	
	GPIO();
	RCC_config();
	CE_OFF();
	CS_OFF();
	SPI2_Init();
	NRF24_Rx_ini();
	CE_ON();
	
	while (1)
	{
		//NRF24_Receive(Rx_data,TX_PLOAD_WIDTH);
		//DelayMili(100);
		
		
	}
}

//=============================================
//uint8_t dt[5];
//uint8_t Rx_data[TX_PLOAD_WIDTH];
//int main (void){
//	
//	GPIO();
//	RCC_config();
//	CE_OFF();
//	CS_OFF();
//	SPI2_Init();
//	NRF24_Rx_ini();
//	while (1)
//	{	
//		dt[0]=NRF24L01P_Read_Register(EN_RXADDR);
//		DelayMili(20);
//		dt[1]=NRF24L01P_Read_Register(EN_AA);
//		DelayMili(20);
//		dt[2]=NRF24L01P_Read_Register(CONFIG);	
//		DelayMili(20);
//		dt[3]=NRF24L01P_Read_Register(STATUS);
//		DelayMili(20);
//		
//	}
//}


