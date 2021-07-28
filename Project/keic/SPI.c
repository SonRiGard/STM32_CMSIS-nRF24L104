#include "SPI.h"

void SPI2_Init (void)
{
//SPI GPIO CONFIG
//	PB0->NSS general output pull-up
//	PB1 general output pull-up //CE => chip enable enable
//	PB13->SCK Master -> Alternate function push-pull
//	PB14->MISO Full duplex / master ->Input floating (/ Input pull-up)
//	PB15->MOSI Full duplex / master ->Alternate function push-pull
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
	
	GPIOB->CRL &= ~GPIO_CRL_CNF0;
	GPIOB->CRL |= GPIO_CRL_MODE0;
	GPIOB->ODR |= GPIO_ODR_ODR0 ;
	
	GPIOB->CRL &= ~GPIO_CRL_CNF1;
	GPIOB->CRL |= GPIO_CRL_MODE1;
	GPIOB->ODR &= ~GPIO_ODR_ODR1;
	
	GPIOB->CRH |= GPIO_CRH_CNF13_1;
	GPIOB->CRH &= ~GPIO_CRH_CNF13_0;
	GPIOB->CRH |= GPIO_CRH_MODE13;
	
	GPIOB->CRH &= ~GPIO_CRH_CNF14_1;
	GPIOB->CRH |= GPIO_CRH_CNF14_0;
	GPIOB->CRH &= ~GPIO_CRH_MODE14;
	
	GPIOB->CRH |= GPIO_CRH_CNF15_1;
	GPIOB->CRH &= ~GPIO_CRH_CNF15_0;
	GPIOB->CRH |= GPIO_CRH_MODE15;
	
	//SPI2_Denit();
	
//Configuring the SPI in master mode
//	Procedure
//1. Select the BR[2:0] bits to define the serial clock baud rate (see SPI_CR1 register).
//2. Select the CPOL and CPHA bits to define one of the four relationships between the
//data transfer and the serial clock (see Figure 240).
//3. Set the DFF bit to define 8- or 16-bit data frame format
//4. Configure the LSBFIRST bit in the SPI_CR1 register to define the frame format.
//5. If the NSS pin is required in input mode, in hardware mode, connect the NSS pin to a
//high-level signal during the complete byte transmit sequence. In NSS software mode,
//set the SSM and SSI bits in the SPI_CR1 register. If the NSS pin is required in output
//mode, the SSOE bit only should be set.
//6. The MSTR and SPE bits must be set (they remain set only if the NSS pin is connected
//to a high-level signal).
	RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
	while ((RCC->APB1ENR & RCC_APB1ENR_SPI2EN) != RCC_APB1ENR_SPI2EN);
	
	SPI2->CR1 |= SPI_CR1_MSTR;//Master configuration
	SPI2->CR1 &= ~SPI_CR1_BIDIMODE;//Full duplex (Transmit and receive)
	SPI2->CR1 &= ~SPI_CR1_RXONLY;//
	SPI2->CR1 &= ~SPI_CR1_DFF;//8-bit data frame format is selected for transmission/reception
	SPI2->CR1 |= (SPI_CR1_BR_0|SPI_CR1_BR_1);//Baud rate control  fPCLK/32 = 36/16
	SPI2->CR1 &= ~SPI_CR1_BR_2;
	SPI2->CR1 &= ~SPI_CR1_CPOL;// CK to 0 when idle, CPOL is reset
	SPI2->CR1 &= ~SPI_CR1_CPHA;// The first clock transition is the first data capture edge, CPHA is reset
	//SPI2->CR2 |= SPI_CR2_SSOE; //SS output is enabled in master mode
	SPI2->CR1 |= SPI_CR1_SSM;//Software slave management enabled
	SPI2->CR1 |= SPI_CR1_SSI;
	SPI2->CR1 &= ~SPI_CR1_LSBFIRST;//MSB transmitted first
	
	//SPI2->CR1 &= ~SPI_CR1_SPE;
	SPI2->CR1 &= ~SPI_CR1_CRCEN;// CRC calculation disabled
	//SPI2->CRCPR = 0X0A;// =10 the polynomial used for the CRC calculation
	SPI2->CR1 |= SPI_CR1_SPE;//Peripheral enabled
}

void SPI2_Denit(void){
	
//	while (!(SPI1->SR & SPI_SR_TXE)){};
//	while (SPI1->SR & SPI_SR_BSY){};
	SPI2->CR1 = 0x0000;
	SPI2->CR2 = 0x0000;
	SPI2->DR = 0x0000;
}

//==============================================
void CS_ON (void)
{
	GPIOB->ODR &= ~GPIO_ODR_ODR0;  
}


void CS_OFF (void)
{
	GPIOB->ODR |= GPIO_ODR_ODR0;
}

// chip enable enable
void CE_ON(void)
{
	GPIOB->ODR |= GPIO_ODR_ODR1;// PB1 set
}

// chip enable disable
void CE_OFF(void)
{
	GPIOB->ODR &= ~GPIO_ODR_ODR1;// PB1 reset
}
void SPI_TransmitReceive(uint8_t *TxData, uint8_t *RxData, uint8_t Size)
{
	//value of Size = so cap byte 1 trans + recei(da bao gom ca RxData Status)
	int i=0;
	/* Don't overwrite in case of HAL_SPI_STATE_BUSY_RX */
	/* Enable SPI peripheral */
	SPI2->CR1 |= SPI_CR1_SPE;//Peripheral enabled	
	/* Transmit and Receive data in 8 Bit mode */
	
	//Write the first data item to be transmitted into the SPI_DR register (this clears the TXE flag).
	SPI2->DR = TxData[i];
	for (i=1;i<Size;i++)
	{
			/* Check TXE flag */
			//Wait until TXE=1 and write the second data item to be transmitted. 
			while((SPI2->SR & SPI_SR_TXE) != SPI_SR_TXE);
			SPI2->DR = TxData[i];
			// wait until RXNE=1 and read the SPI_DR to get the first received data
			while(!(READ_BIT(SPI2->SR, SPI_SR_RXNE) == (SPI_SR_RXNE))) {}
			RxData[i-1] = SPI2->DR;
	}		
			//until the n–1 received data.
			if (i==(Size-1))
			{
				//Wait until RXNE=1 and read the last received data
				while(!(READ_BIT(SPI2->SR, SPI_SR_RXNE) == (SPI_SR_RXNE))) {}
				RxData[i] = SPI2->DR;
				//Wait until TXE=1 and then wait until BSY=0 before disabling the SPI.
				//while(!(READ_BIT(SPI2->SR, SPI_SR_TXE) == (SPI_SR_TXE))) {}
				while (SPI2->SR & SPI_SR_BSY);					
			}
	/* Clear overrun flag in 2 Lines communication mode because received is not read */
	(void)SPI2->DR;
	(void)SPI2->SR;
	
	//disabling the SPI.
	SPI2->CR1 &= ~SPI_CR1_SPE;//Peripheral disabled
}

void SPI_Transmit (uint8_t *Data, uint8_t size)
{
	int8_t i=0;
//	/* Disable SPI Peripheral before set 1Line direction (BIDIOE bit) */
//	SPI2->CR1 &= ~SPI_CR1_SPE;//Peripheral disabled
//	SPI2->CR1 |= SPI_CR1_BIDIOE;
//	SPI2->CR1 &= ~SPI_CR1_RXONLY;
	//enable again
	SPI2->CR1 |= SPI_CR1_SPE;//Peripheral enabled	
	/* Check TXE flag */
	for(i=0;i<size;i++) 
	{
		if (i<(size-1))
		{
			while((SPI2->SR & SPI_SR_TXE) != SPI_SR_TXE);
			SPI2->DR = Data[i];
	  }else 
		{
			while((SPI2->SR & SPI_SR_TXE) != SPI_SR_TXE);
			SPI2->DR = Data[i];
			while((SPI2->SR & SPI_SR_TXE) != SPI_SR_TXE) {}
			while (SPI2->SR & SPI_SR_BSY);/* Control the BSY flag */
		}
	}
	/* Clear overrun flag in 2 Lines communication mode because received is not read */
	(void)SPI2->DR;
	(void)SPI2->SR;
	SPI2->CR1 &= ~SPI_CR1_SPE;
}

void SPI_Receiver (uint8_t *Data , uint8_t size)
{
	int i=0;
	SPI2->CR1 |= SPI_CR1_SPE;//Peripheral enabled	
	SPI_TransmitReceive(Data,Data,size);
	//Disable SPI Peripheral before set 1Line direction (BIDIOE bit) 
	//SPI2->CR1 &= ~SPI_CR1_SPE;//Peripheral enabled
	//RXNE behavior in receive-only mode
	//SPI2->CR1 &= ~SPI_CR1_BIDIOE;//BIDIOE=1
	//SPI2->CR1 |= SPI_CR1_RXONLY;//RXONLY=1
	//enable again
	for(i=0; i<size; i++)
	{
		while(!(READ_BIT(SPI1->SR, SPI_SR_RXNE) == (SPI_SR_RXNE))) {}
		Data[i] = SPI1->DR;
	}
	//Disable SPI Peripheral =>end Rx transaction
	SPI2->CR1 &= ~SPI_CR1_SPE;//Peripheral enabled
}



//========================================================================================================
//========================================================================================================
//========================================================================================================

void SPI1_Init (void)
{
//SPI GPIO CONFIG
//	PB0->NSS general output pull-up
//	PB1 general output pull-up //CE => chip enable enable
//	PA5->SCK Master -> Alternate function push-pull
//	PA6->MISO Full duplex / master ->Input floating (/ Input pull-up)
//	PB7->MOSI Full duplex / master ->Alternate function push-pull
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	
//	GPIOB->CRL &= ~GPIO_CRL_CNF0;
//	GPIOB->CRL |= GPIO_CRL_MODE0;
//	GPIOB->ODR |= GPIO_ODR_ODR0 ;
//	
//	GPIOB->CRL &= ~GPIO_CRL_CNF1;
//	GPIOB->CRL |= GPIO_CRL_MODE1;
//	GPIOB->ODR &= ~GPIO_ODR_ODR1;
	
	GPIOA->CRL &= ~GPIO_CRL_CNF5_1;
	GPIOA->CRL |= GPIO_CRL_CNF5_0;
	GPIOA->CRL &= ~GPIO_CRL_MODE5;
	
	GPIOA->CRL |= GPIO_CRL_CNF6_1;
	GPIOA->CRL &= ~GPIO_CRL_CNF6_0;
	GPIOA->CRL |= GPIO_CRL_MODE6;
	
	GPIOA->CRL &= ~GPIO_CRL_CNF7_1;
	GPIOA->CRL |= GPIO_CRL_CNF7_0;
	GPIOA->CRL &= ~GPIO_CRL_MODE7;
	
	SPI1_Denit();
	
//Configuring the SPI in master mode
//	Procedure
//1. Select the BR[2:0] bits to define the serial clock baud rate (see SPI_CR1 register).
//2. Select the CPOL and CPHA bits to define one of the four relationships between the
//data transfer and the serial clock (see Figure 240).
//3. Set the DFF bit to define 8- or 16-bit data frame format
//4. Configure the LSBFIRST bit in the SPI_CR1 register to define the frame format.
//5. If the NSS pin is required in input mode, in hardware mode, connect the NSS pin to a
//high-level signal during the complete byte transmit sequence. In NSS software mode,
//set the SSM and SSI bits in the SPI_CR1 register. If the NSS pin is required in output
//mode, the SSOE bit only should be set.
//6. The MSTR and SPE bits must be set (they remain set only if the NSS pin is connected
//to a high-level signal).
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	SPI1->CR1 &= ~SPI_CR1_MSTR;//Slave configuration
	SPI1->CR1 &= ~SPI_CR1_BIDIMODE;//Full duplex (Transmit and receive)
	SPI1->CR1 &= ~SPI_CR1_RXONLY;//
	SPI1->CR1 &= ~SPI_CR1_DFF;//8-bit data frame format is selected for transmission/reception
	SPI1->CR1 |= SPI_CR1_BR_2;//Baud rate control  fPCLK/32 =2.25
	SPI1->CR1 &= ~SPI_CR1_BR_2;
	SPI1->CR1 &= ~SPI_CR1_CPOL;// CK to 0 when idle, CPOL is reset
	SPI1->CR1 &= ~SPI_CR1_CPHA;// The first clock transition is the first data capture edge, CPHA is reset
	//SPI2->CR2 |= SPI_CR2_SSOE; //SS output is enabled in master mode
	SPI1->CR1 |= SPI_CR1_SSM;//Software slave management enabled
	SPI1->CR1 |= SPI_CR1_SSI;
	SPI1->CR1 &= ~SPI_CR1_LSBFIRST;//MSB transmitted first
	
	//SPI2->CR1 &= ~SPI_CR1_SPE;
	SPI1->CR1 &= ~SPI_CR1_CRCEN;// CRC calculation disabled
	//SPI1->CRCPR = 0X0A;// =10 the polynomial used for the CRC calculation
	SPI1->CR1 |= SPI_CR1_SPE;//Peripheral enabled
}

void SPI1_Denit(void){
	
//	while (!(SPI1->SR & SPI_SR_TXE)){};
//	while (SPI1->SR & SPI_SR_BSY){};
	SPI1->CR1 = 0x0000;
	SPI1->CR2 = 0x0000;
	SPI1->DR = 0x0000;
}

