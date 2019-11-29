/*
********************************************************************************
* COPYRIGHT(c) ЗАО «ЧИП и ДИП», 2017 - 2019
* 
* Программное обеспечение предоставляется на условиях «как есть» (as is).
* При распространении указание автора обязательно.
********************************************************************************
*/


#ifndef __I2C_H
#define __I2C_H


#include "Board.h"


#if defined(TAS3251_DSP) || defined(TAS3251_DSPx2)
  #define   INTERNAL_ADDRESS_BYTES_COUNT     1
#elif defined(ADAU1761_DSP) || defined(ADAU1701_DSP)
  #define   INTERNAL_ADDRESS_BYTES_COUNT     2
#endif

#define   I2C_TAS3251_1                    0
#define   I2C_TAS3251_2                    1


void I2C_Init(uint8_t Address);

void I2C_Write(uint16_t StartAddress, uint16_t BytesNum, uint8_t *DataArray);

void I2C_Read(uint16_t StartAddress, uint16_t BytesNum, uint8_t *DataArray);

#if defined(ADAU1761_DSP)
void I2C_ADAU_CLEAR_REGISTER_BLOCK( uint16_t StartAddress, uint16_t BytesNum );
#endif

#if defined(TAS3251_DSPx2)
void I2C_ChangeGPIOs(uint8_t I2CNum);
#endif

#endif //__I2C_H


