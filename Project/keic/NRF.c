#include "NRF.h"
#include "SPI.h"
#include "TIM2.h"
 
void NRF24_ini(void)
{
  CE_OFF();
  DelayMicro(5000); 
	// Set PWR_UP bit, enable CRC(1 byte) &Prim_RX:0 (Transmitter)
	NRF24_WriteReg(CONFIG, 0x0a);
	DelayMicro(5000);
	NRF24_WriteReg(EN_AA, 0x02); // Enable Pipe1
	NRF24_WriteReg(EN_RXADDR, 0x02); // Enable Pipe1
	NRF24_WriteReg(SETUP_AW, 0x01); // Setup address width=3 bytes
	NRF24_WriteReg(SETUP_RETR, 0x5F); // // 1500us, 15 retrans
	void NRF24_ToggleFeatures (void);//use funtion with received (Rx)
	NRF24_WriteReg(FEATURE, 0);
	NRF24_WriteReg(DYNPD, 0);
	NRF24_WriteReg(STATUS, 0x70); //Reset flags for IRQ
	NRF24_WriteReg(RF_CH, 76); // f = 2476 MHz
	NRF24_WriteReg(RF_SETUP, 0x06); //TX_PWR:0dBm, Datarate:1Mbps
}

uint8_t NRF24_ReadReg(uint8_t addr)
{
  uint8_t RxData[1];
	uint8_t TxData[2];
	uint8_t cmd;
	TxData[0]=addr;
  CS_ON();
  SPI_TransmitReceive(TxData,RxData,1);

  if (addr!=STATUS)
  {
   cmd=0xFF;
   SPI_TransmitReceive(&cmd,RxData,1);
  }
  CS_OFF();
  return RxData[0];
}

void NRF24_WriteReg(uint8_t addr, uint8_t dt)
{
	uint8_t TxData[1];
	addr |= W_REGISTER;
  CS_ON();
	TxData[0]=addr;
	SPI_Transmit(TxData,1);
	TxData[0]=dt;
	SPI_Transmit(TxData,1);
  CS_OFF();
}
void NRF24_ToggleFeatures (void)
{
	uint8_t TxData[1];
	CS_ON();
	TxData[0]=ACTIVATE;
	SPI_Transmit(TxData,1);
	DelayMicro(1);
	TxData[0]=0x73;
	SPI_Transmit(TxData,1);
	CS_OFF();
}

 