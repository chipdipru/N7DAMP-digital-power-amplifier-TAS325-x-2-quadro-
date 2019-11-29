/*
********************************************************************************
* COPYRIGHT(c) ��� ���� � ��ϻ, 2017 - 2019
* 
* ����������� ����������� ��������������� �� �������� ���� ����� (as is).
* ��� ��������������� �������� ������ �����������.
********************************************************************************
*/


#include "I2C.h"


void I2C_Init(uint8_t Address)
{
  I2C_GPIO->OTYPER |= (1 << I2C_SCL_PIN) | (1 << I2C_SDA_PIN);
  I2C_GPIO->AFR[0] |= (I2C_SCL_AF << (4 * (I2C_SCL_PIN))) | (I2C_SDA_AF << (4 * (I2C_SDA_PIN)));
  I2C_GPIO->MODER |= (2 << (2 * I2C_SCL_PIN)) | (2 << (2 * I2C_SDA_PIN));
  
#if defined(TAS3251_DSPx2)
  I2C2_GPIO->OTYPER |= (1 << I2C2_SCL_PIN) | (1 << I2C2_SDA_PIN);
  I2C2_GPIO->AFR[1] |= (I2C2_SCL_AF << (4 * (I2C2_SCL_PIN - 8))) | (I2C2_SDA_AF << (4 * (I2C2_SDA_PIN - 8)));
#endif
  
  RCC->CFGR3 |= RCC_CFGR3_I2C1SW_SYSCLK;
  RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
  //I2C_PORT->TIMINGR = (uint32_t)0x10420F13; //100 ��� @ 8 MHz
  //I2C_PORT->TIMINGR = (uint32_t)0x00310309; //400 ��� @ 8 MHz
  I2C_PORT->TIMINGR = (uint32_t)0x50330309; //400 ��� @ 48 MHz
  //I2C_PORT->TIMINGR = (uint32_t)0x50100103; //1 ��� @ 48 MHz
  I2C_PORT->CR1 = I2C_CR1_PE;
  
  I2C_PORT->CR2 |= Address << 1;
}
//------------------------------------------------------------------------------
void I2C_Write(uint16_t StartAddress, uint16_t BytesNum, uint8_t *DataArray)
{
  I2C_PORT->CR2 &= ~(I2C_CR2_NBYTES | I2C_CR2_RD_WRN);
  I2C_PORT->CR2 |= ((BytesNum + INTERNAL_ADDRESS_BYTES_COUNT) << 16);
  
#if (INTERNAL_ADDRESS_BYTES_COUNT == 2)  
  I2C_PORT->TXDR = StartAddress >> 8;
  I2C_PORT->CR2 |= I2C_CR2_START;
  while(!(I2C_PORT->ISR & I2C_ISR_TXE)); //��� ���������� �������� �������
                                         //� ����� � �������
#endif
  
  I2C_PORT->TXDR = StartAddress;
#if (INTERNAL_ADDRESS_BYTES_COUNT == 1)  
  I2C_PORT->CR2 |= I2C_CR2_START;
#endif  
  while(!(I2C_PORT->ISR & I2C_ISR_TXE)); //��� ���������� �������� �������
                                         //� ����� � �������
  
  while(BytesNum)
  {
    I2C_PORT->TXDR = *DataArray;
    while(!(I2C_PORT->ISR & I2C_ISR_TXE)); //��� ���������� �������� �������
                                           //� ����� � �������
    *DataArray++;
    BytesNum--;
  }
  
  I2C_PORT->CR2 |= I2C_CR2_STOP;
}
//------------------------------------------------------------------------------
void I2C_Read(uint16_t StartAddress, uint16_t BytesNum, uint8_t *DataArray)
{
  I2C_PORT->CR2 &= ~(I2C_CR2_NBYTES | I2C_CR2_RD_WRN);
  I2C_PORT->CR2 |= (INTERNAL_ADDRESS_BYTES_COUNT << 16);
  
#if (INTERNAL_ADDRESS_BYTES_COUNT == 2)
  I2C_PORT->TXDR = StartAddress >> 8;
  I2C_PORT->CR2 |= I2C_CR2_START;
  while(!(I2C_PORT->ISR & I2C_ISR_TXE)); //��� ���������� �������� �������
                                         //� ����� � �������
#endif
  
  I2C_PORT->TXDR = StartAddress;
#if (INTERNAL_ADDRESS_BYTES_COUNT == 1)  
  I2C_PORT->CR2 |= I2C_CR2_START;
#endif    
  while(!(I2C_PORT->ISR & I2C_ISR_TXE)); //��� ���������� �������� �������
                                         //� ����� � �������
  
  I2C_PORT->CR2 |= I2C_CR2_RD_WRN;
  I2C_PORT->CR2 &= ~I2C_CR2_NBYTES;
  I2C_PORT->CR2 |= ((BytesNum) << 16);
  I2C_PORT->CR2 |= I2C_CR2_START;
    
  do
  {
    while(!(I2C_PORT->ISR & I2C_ISR_RXNE)); //��� ���������� �������� �������
                                            //� ����� � �������
    *DataArray = I2C_PORT->RXDR;
    *DataArray++;
    BytesNum--;
  }
  while(BytesNum);
  
  I2C_PORT->CR2 |= I2C_CR2_STOP;
  I2C_PORT->ICR |= I2C_ICR_STOPCF;
}
//------------------------------------------------------------------------------
#if defined(ADAU1761_DSP)
void I2C_ADAU_CLEAR_REGISTER_BLOCK( uint16_t StartAddress, uint16_t BytesNum )
{
  I2C_PORT->CR2 &= ~(I2C_CR2_NBYTES | I2C_CR2_RD_WRN);
  I2C_PORT->CR2 |= ((BytesNum + 2) << 16);
  
  I2C_PORT->TXDR = StartAddress >> 8;
  I2C_PORT->CR2 |= I2C_CR2_START;
  while(!(I2C_PORT->ISR & I2C_ISR_TXE)); //��� ���������� �������� �������
                                         //� ����� � �������
  I2C_PORT->TXDR = StartAddress;
  while(!(I2C_PORT->ISR & I2C_ISR_TXE)); //��� ���������� �������� �������
                                         //� ����� � �������
  
  while(BytesNum)
  {
    I2C_PORT->TXDR = 0x00;
    while(!(I2C_PORT->ISR & I2C_ISR_TXE)); //��� ���������� �������� �������
                                           //� ����� � �������
    BytesNum--;
  }
  
  I2C_PORT->CR2 |= I2C_CR2_STOP;
}
#endif
//------------------------------------------------------------------------------
#if defined(TAS3251_DSPx2)
void I2C_ChangeGPIOs(uint8_t I2CNum)
{
  if (I2CNum == I2C_TAS3251_1)
  {
    I2C2_GPIO->MODER &= ~((2 << (2 * I2C2_SCL_PIN)) | (2 << (2 * I2C2_SDA_PIN)));
    I2C_GPIO->MODER |= (2 << (2 * I2C_SCL_PIN)) | (2 << (2 * I2C_SDA_PIN));
  }
  else
  {
    I2C_GPIO->MODER &= ~((2 << (2 * I2C_SCL_PIN)) | (2 << (2 * I2C_SDA_PIN)));
    I2C2_GPIO->MODER |= (2 << (2 * I2C2_SCL_PIN)) | (2 << (2 * I2C2_SDA_PIN));
  }
}
#endif


