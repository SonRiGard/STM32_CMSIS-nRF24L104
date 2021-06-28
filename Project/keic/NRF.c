#include "NRF.h"
#include "SPI.h"
#include "TIM2.h"

#define TX_ADR_WIDTH 3
#define TX_PLOAD_WIDTH 2
uint8_t TX_ADDRESS[TX_ADR_WIDTH] = {0xb3,0xb4,0x01};
uint8_t RX_BUF[TX_PLOAD_WIDTH] = {0};


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
	NRF24_ToggleFeatures();//use funtion with received (Rx)
	NRF24_WriteReg(FEATURE, 0);
	NRF24_WriteReg(DYNPD, 0);
	NRF24_WriteReg(STATUS, 0x70); //Reset flags for IRQ
	NRF24_WriteReg(RF_CH, 76); // f = 2476 MHz
	NRF24_WriteReg(RF_SETUP, 0x06); //TX_PWR:0dBm, Datarate:1Mbps
	NRF24_Write_Buf(TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);
	NRF24_Write_Buf(RX_ADDR_P1, TX_ADDRESS, TX_ADR_WIDTH);
	NRF24_WriteReg(RX_PW_P1, TX_PLOAD_WIDTH); //Number of bytes in RX payload in data pipe 1
	NRF24L01_RX_Mode();	
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
//can tao mot point de luu gia tri doc RxData
void NRF24_Read_Buf(uint8_t addr,uint8_t *RxData,uint8_t bytes)
{
  uint8_t TxData[1];
	TxData[0] = addr;
	CS_ON();	
  SPI_Transmit(TxData,1);
  SPI_Receiver(RxData,bytes);
  CS_OFF();
}

void NRF24_Write_Buf(uint8_t addr,uint8_t *pBuf,uint8_t bytes)
{
	uint8_t TxData[1];
	TxData[0] = addr|W_REGISTER;
  CS_ON();
	SPI_Transmit(TxData,1);
  DelayMicro(1);
  SPI_Transmit(pBuf,bytes);
  CS_OFF();
}
//------------------------------------------------
void NRF24_FlushRX(void)
{
  uint8_t dt[1] = {FLUSH_RX};
  CS_ON();
  SPI_Transmit(dt,1);
  DelayMicro(1);
  CS_OFF();
}
//------------------------------------------------
void NRF24_FlushTX(void)
{
  uint8_t dt[1] = {FLUSH_TX};
  CS_ON();
  SPI_Transmit(dt,1);
  DelayMicro(1);
  CS_OFF();
}
//------------------------------------------------
//------------------------------------------------

void NRF24L01_RX_Mode(void)
{
	//set the PWR_UP and PRIM_RX bit to 1
  uint8_t regval=0x00;
  regval = NRF24_ReadReg(CONFIG);
  regval |= (1<<PWR_UP)|(1<<PRIM_RX);
  NRF24_WriteReg(CONFIG,regval);
	// Disable Pipe1
	NRF24_WriteReg(EN_AA, 0x00); 
	//Setup address width=3 bytes
	NRF24_WriteReg(SETUP_AW, 0x01);
	// f = 2476 MHz the same frequency chanel with Tx 
	NRF24_WriteReg(RF_CH, 76); 
	//set the data rate TX_PWR:0dBm, Datarate:1Mbps 
	NRF24_WriteReg(RF_SETUP, 0x06); 
  CE_ON();
  DelayMicro(150); //wait 130us
  // Flush buffers
  NRF24_FlushRX();
  NRF24_FlushTX();
}
//------------------------------------------------
void NRF24L01_TX_Mode (void)
{
	//set the PRIM_RX bit to 0
	uint8_t regval=0x00;
  regval = NRF24_ReadReg(CONFIG);
  regval &= ~(1<<PRIM_RX);
	NRF24_WriteReg(CONFIG,regval);
	//1500us, 15 retrans
	NRF24_WriteReg(SETUP_RETR, 0x5F); 
	//Setup address width=3 bytes
	NRF24_WriteReg(SETUP_AW, 0x01);
	// f = 2476 MHz the same frequency chanel with Tx 
	NRF24_WriteReg(RF_CH, 76); 
	//set the data rate TX_PWR:0dBm, Datarate:1Mbps 
	NRF24_WriteReg(RF_SETUP, 0x06); 
	//set the PWR_UP to high
  regval = NRF24_ReadReg(CONFIG);
  regval |= (1<<PWR_UP);
	NRF24_WriteReg(CONFIG,regval);
		
	CE_ON();//Pulse CE to transmit packet
  DelayMicro(150); //wait 130us
  // Flush buffers
  NRF24_FlushRX();
  NRF24_FlushTX();
}



