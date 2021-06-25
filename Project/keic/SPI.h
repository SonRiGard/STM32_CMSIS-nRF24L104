#include "stm32f10x.h"

void SPI2_Denit(void);
void SPI2_Init(void);
void CS_ON (void);
void CS_OFF (void);
void CE_OFF(void);
void CE_ON(void);
void SPI_TransmitReceive(uint8_t *TxData, uint8_t *RxData, uint8_t Size);
void SPI_Transmit (uint8_t *Data, uint8_t size);
void SPI_Receiver (uint8_t *Data , uint8_t size);

