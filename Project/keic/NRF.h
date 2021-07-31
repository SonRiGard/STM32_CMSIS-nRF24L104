#include "stm32f10x.h"

//--------------------------------------------------------------------------
#define ACTIVATE 0x50 //
#define RD_RX_PLOAD 0x61 // Define RX payload register address
#define WR_TX_PLOAD 0xA0 // Define TX payload register address
#define FLUSH_TX 0xE1
#define FLUSH_RX 0xE2
//------------------------------------------------
#define CONFIG 0x00 //'Config' register address
#define EN_AA 0x01 //'Enable Auto Acknowledgment' register address
#define EN_RXADDR 0x02 //'Enabled RX addresses' register address
#define SETUP_AW 0x03 //'Setup address width' register address
#define SETUP_RETR 0x04 //'Setup Auto. Retrans' register address
#define RF_CH 0x05 //'RF channel' register address
#define RF_SETUP 0x06 //'RF setup' register address
#define STATUS 0x07 //'Status' register address
#define OBSERVE_TX 0x08 //'Transmit observe' register
#define RX_ADDR_P0 0x0A //'RX address pipe0' register address
#define RX_ADDR_P1 0x0B //'RX address pipe1' register address
#define TX_ADDR 0x10 //'TX address' register address
#define RX_PW_P0 0x11 //'RX payload width, pipe0' register address
#define RX_PW_P1 0x12 //'RX payload width, pipe1' register address
#define FIFO_STATUS 0x17 //'FIFO Status Register' register address
#define DYNPD 0x1C
#define FEATURE 0x1D
#define RX_ADDR_P1  0x0B
#define RX_ADDR_P2  0x0C
#define RX_ADDR_P3  0x0D
#define RX_ADDR_P4  0x0E
#define RX_ADDR_P5  0x0F
#define RX_PW_P0    0x11
#define RX_PW_P1    0x12
#define RX_PW_P2    0x13
#define RX_PW_P3    0x14
#define RX_PW_P4    0x15
#define RX_PW_P5    0x16
//------------------------------------------------
#define PRIM_RX 0x00 //RX/TX control (1: PRX, 0: PTX)
#define PWR_UP 0x01 //1: POWER UP, 0:POWER DOWN
#define RX_DR 0x40 //Data Ready RX FIFO interrupt
#define TX_DS 0x20 //Data Sent TX FIFO interrupt
#define MAX_RT 0x10 //Maximum number of TX retransmits interrupt
//-----------------------------------------------
#define W_REGISTER 0x20 //command Write
//-----------------------------------------------------------------------
void NRF24_Tx_ini(void);
void NRF24_Transmit (uint8_t *data);
void NRF24_Rx_ini(void);
void NRF24_Receive (uint8_t *data,uint8_t size);
void NRF24_reset(uint8_t REG);
uint8_t NRF24_ReadReg(uint8_t addr);
void NRF24_WriteReg(uint8_t addr, uint8_t dt);
void NRF24_ToggleFeatures (void);
void NRF24_Write_Buf(uint8_t addr,uint8_t *RxData,uint8_t bytes);
void NRF24_Read_Buf(uint8_t addr,uint8_t *RxData,uint8_t bytes);
void NRF24_FlushTX(void);
void NRF24_FlushRX(void);
void NRF24L01_RX_Mode(void);
void NRF24L01_TX_Mode (void);
uint8_t read_1_reg(uint8_t reg);
void write_1_reg(uint8_t reg,uint8_t data);
void NRF24L01P_SingleWrite_Register(uint8_t RegAdd, uint8_t Data);

uint8_t NRF24L01P_Read_Register(uint8_t RegAdd);



