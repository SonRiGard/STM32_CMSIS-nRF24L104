#include "NRF.h"
#include "SPI.h"
#include "TIM2.h"

#define TX_ADR_WIDTH 3
#define TX_PLOAD_WIDTH 2
uint8_t TX_ADDRESS[TX_ADR_WIDTH] = {0xb3,0xb4,0x01};
uint8_t RX_BUF[TX_PLOAD_WIDTH] = {0};


void NRF24_reset(uint8_t REG )
{
	CE_OFF();
  DelayMicro(5000); 
	
	if (REG == STATUS) 
	{
		NRF24_WriteReg(STATUS, 0x00);
	}
	
	else 	if (REG == FIFO_STATUS)
	{
		NRF24_WriteReg(FIFO_STATUS,0x11);
	}				
	
	else 
	{
		NRF24_WriteReg(CONFIG,0x08);
		NRF24_WriteReg(EN_AA, 0x3F);
		NRF24_WriteReg(EN_RXADDR, 0x03);
		NRF24_WriteReg(SETUP_AW, 0x03);
		NRF24_WriteReg(SETUP_RETR, 0x03);
		NRF24_WriteReg(RF_CH, 0x02);
		NRF24_WriteReg(RF_SETUP, 0x0E);
		NRF24_WriteReg(STATUS, 0x00);
		NRF24_WriteReg(OBSERVE_TX, 0x00);
		NRF24_WriteReg(RX_ADDR_P2, 0xC3);
		NRF24_WriteReg(RX_ADDR_P3, 0xC4);
		NRF24_WriteReg(RX_ADDR_P4, 0xC5);
		NRF24_WriteReg(RX_ADDR_P5, 0xC6);
		NRF24_WriteReg(RX_PW_P0, 0);
		NRF24_WriteReg(RX_PW_P1, 0);
		NRF24_WriteReg(RX_PW_P2, 0);
		NRF24_WriteReg(RX_PW_P3, 0);
		NRF24_WriteReg(RX_PW_P4, 0);
		NRF24_WriteReg(RX_PW_P5, 0);
		NRF24_WriteReg(FIFO_STATUS, 0x11);
		NRF24_WriteReg(DYNPD, 0);
		NRF24_WriteReg(FEATURE, 0);			
	}

		CE_ON();
}


void NRF24_Tx_ini(void)
{

	uint8_t config = 0;
	CE_OFF();
	NRF24_reset(0);
	NRF24_WriteReg(EN_AA, 0);
	NRF24_WriteReg(EN_RXADDR, 0);
	NRF24_WriteReg(SETUP_AW, 0x03);// 5 Bytes for the TX/RX address
	NRF24_WriteReg(SETUP_RETR, 0x05);//retransmit 5 time + wait 250 us
	NRF24_WriteReg(RF_CH, 76);// period 2476 MHz	
	NRF24_WriteReg(RF_SETUP, 0x06 );//TX_PWR:0dBm, Datarate:1Mbps
	config = NRF24_ReadReg(CONFIG);
	config &= 0xF2;
	NRF24_WriteReg (CONFIG, config);	
	CE_ON();	
}

void NRF24_Transmit (uint8_t *data)
{
	uint8_t REG[1] = {WR_TX_PLOAD};
	uint8_t fifostatus = 0;
	CS_ON();
	
	SPI_Transmit(REG,1);
	SPI_Transmit(data,TX_PLOAD_WIDTH);
	
	CS_OFF();
	
	//delay 1
	fifostatus = NRF24L01P_Read_Register(FIFO_STATUS);
	if ((fifostatus & 0x10)==0x10)
			{	
				REG[0]=FLUSH_TX;
				SPI_Transmit(REG,1);
				
				// reset FIFO_STATUS
				NRF24_reset (FIFO_STATUS);				
			}
}


void NRF24_Rx_ini(void)
{
	uint8_t config = 0;
	CE_OFF();
	NRF24_reset(STATUS);
	NRF24_WriteReg(EN_RXADDR, 0x01);
	NRF24_WriteReg(SETUP_AW, 0x03);// 5 Bytes for the TX/RX address
	NRF24_WriteReg(SETUP_RETR, 0x05);//retransmit 5 time + wait 250 us
	NRF24_WriteReg(RF_CH, 76);// period 2476 MHz	
	NRF24_WriteReg(RF_SETUP, 0x06 );//TX_PWR:0dBm, Datarate:1Mbps
	//NRF24_WriteReg(RX_ADDR_P1, 0xC2);// Write the Pipe1 LSB address
	NRF24_WriteReg(RX_PW_P1, TX_PLOAD_WIDTH);//  set numbers bit payload size for pipe 1 = TX_PLOAD_WIDTH
	
	config = NRF24_ReadReg(CONFIG);
	config |= 0x03;
	NRF24_WriteReg (CONFIG, config);
	CE_ON();	
}

void NRF24_Receive (uint8_t *data,uint8_t size)
{
	uint8_t count=0 ;
	uint8_t REG[1]={RD_RX_PLOAD};
	CS_ON();
	
	SPI2->CR1 |= SPI_CR1_SPE;

	while(!(READ_BIT(SPI2->SR, SPI_SR_TXE) == (SPI_SR_TXE))) {}
	SPI2->DR = REG[0];
	if(REG[0] == STATUS){
		while(!(READ_BIT(SPI2->SR, SPI_SR_RXNE) == (SPI_SR_RXNE))) {}
		data[0] =SPI2->DR;
	}
	else{
		while(!(READ_BIT(SPI2->SR, SPI_SR_TXE) == (SPI_SR_TXE))) {}
		while (SPI2->SR & SPI_SR_BSY);
		SPI2->CR1 |= SPI_CR1_RXONLY;

		while(!(READ_BIT(SPI2->SR, SPI_SR_RXNE) == (SPI_SR_RXNE))) {}
		(void) SPI2->DR;
		
		while (count < size ) 
		{
		while(!(READ_BIT(SPI2->SR, SPI_SR_RXNE) == (SPI_SR_RXNE))) {}
		data[count] = SPI2->DR;
		count++;
		}
		SPI2->CR1 &= ~SPI_CR1_RXONLY;
	}


	CS_OFF();
	//dalay1
	REG[0]=FLUSH_RX;
	SPI_Transmit (REG, 1);	
}


uint8_t NRF24_ReadReg(uint8_t addr)
{
	
  uint8_t RxData[1];
	uint8_t TxData[2];
	uint8_t cmd;
	TxData[0]=addr;
  RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
	CS_ON();
  SPI_TransmitReceive(TxData,RxData,1);
	
  if (addr==STATUS)
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


uint8_t NRF24L01P_Read_Register(uint8_t RegAdd){
	uint8_t READ_REG = 0;
	SPI2->CR1 |= SPI_CR1_SPE;
 	CS_ON();

	while(!(READ_BIT(SPI2->SR, SPI_SR_TXE) == (SPI_SR_TXE))) {}
	SPI2->DR = RegAdd;
	if(RegAdd == STATUS){
		while(!(READ_BIT(SPI2->SR, SPI_SR_RXNE) == (SPI_SR_RXNE))) {}
		READ_REG = SPI2->DR;
	}
	else{
		while(!(READ_BIT(SPI2->SR, SPI_SR_TXE) == (SPI_SR_TXE))) {}
		while (SPI2->SR & SPI_SR_BSY);
		SPI2->CR1 |= SPI_CR1_RXONLY;

		while(!(READ_BIT(SPI2->SR, SPI_SR_RXNE) == (SPI_SR_RXNE))) {}
		(void) SPI2->DR;


		while(!(READ_BIT(SPI2->SR, SPI_SR_RXNE) == (SPI_SR_RXNE))) {}
		READ_REG = SPI2->DR;
		CS_OFF();
		SPI2->CR1 &= ~SPI_CR1_RXONLY;
	}

	return READ_REG;

}




/*

//cai nay tu viet va chua dung toi 
//------------------------------------------------
//+++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++
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
//=====================================================================
//=====================================================================
//=====================================================================
void write_1_reg(uint8_t reg,uint8_t data)
{
	uint8_t addr=0;
	uint8_t tmp_over;
	addr |= (reg|W_REGISTER);
	CS_ON();
	SPI2->DR = addr;	
	while((SPI2->SR & SPI_SR_TXE)!=SPI_SR_TXE);
	SPI2->DR = (uint8_t)data;
	while((SPI2->SR & SPI_SR_TXE)!=SPI_SR_TXE);
	while((SPI2->SR & SPI_SR_BSY)==SPI_SR_BSY);
	tmp_over=SPI2->DR;
	tmp_over=SPI2->SR;
	CS_OFF();	
}


void read_1_reg(uint8_t reg,uint8_t *Rxdata)
{
	uint8_t addr=0;
	uint32_t tmp_over;
	addr = reg;
	CS_ON();
	SPI2->DR = addr;
	while((SPI2->SR & SPI_SR_TXE)!=SPI_SR_TXE);
	while((SPI2->SR & SPI_SR_BSY)==SPI_SR_BSY);
	tmp_over=SPI2->DR;
	tmp_over=SPI2->SR;
//	while ((SPI2->SR & SPI_SR_RXNE)!=SPI_SR_RXNE);
	SPI2->DR=0x00;
	while((SPI2->SR & SPI_SR_TXE)!=SPI_SR_TXE);
	while ((SPI2->SR & SPI_SR_RXNE)!=SPI_SR_RXNE);
	//SPI2->CR1 &= ~SPI_CR1_SPE;//Disable the SPI (SPE=0)
	Rxdata[0]=SPI2->DR;
	CS_OFF();	
}
//+++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++
*/

