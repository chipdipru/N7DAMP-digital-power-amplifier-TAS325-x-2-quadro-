/*
********************************************************************************
* COPYRIGHT(c) ЗАО «ЧИП и ДИП», 2019
* 
* Программное обеспечение предоставляется на условиях «как есть» (as is).
* При распространении указание автора обязательно.
********************************************************************************
*/


#ifndef __BOARD_H
#define __BOARD_H


#include "stm32f042x6.h"


//#define   TAS3251_DSP
//#define   ADAU1761_DSP
//#define   ADAU1701_DSP
#define   TAS3251_DSPx2

//I2C
#define   I2C_SCL_PIN                      6
#define   I2C_SCL_PORT                     GPIOB
#define   I2C_SCL_AF                       1
#define   I2C_SDA_PIN                      7
#define   I2C_SDA_PORT                     GPIOB
#define   I2C_SDA_AF                       1
#define   I2C_GPIO                         GPIOB
#define   I2C_PORT                         I2C1

//I2C2
#define   I2C2_SCL_PIN                     9
#define   I2C2_SCL_PORT                    GPIOA
#define   I2C2_SCL_AF                      4
#define   I2C2_SDA_PIN                     10
#define   I2C2_SDA_PORT                    GPIOA
#define   I2C2_SDA_AF                      4
#define   I2C2_GPIO                        GPIOA

//ADC
#define   ADC_0_PIN                        0
#define   ADC_1_PIN                        1
#define   ADC_2_PIN                        2
#define   ADC_3_PIN                        3
#define   ADC_4_PIN                        4
#define   ADC_5_PIN                        5
#define   ADC_6_PIN                        6
#define   ADC_7_PIN                        7
#define   ADC_GPIO                         GPIOA
#define   ADC_8_PIN                        0
#define   ADC_9_PIN                        1
#define   ADC_GPIO2                        GPIOB
#define   ADC_USED                         ADC1
#define   ADC_ENR                          APB2ENR
#define   ADC_CLK_EN                       RCC_APB2ENR_ADCEN
#define   ADC_0_CHNL                       ADC_CHSELR_CHSEL0
#define   ADC_1_CHNL                       ADC_CHSELR_CHSEL1
#define   ADC_2_CHNL                       ADC_CHSELR_CHSEL2
#define   ADC_3_CHNL                       ADC_CHSELR_CHSEL3
#define   ADC_4_CHNL                       ADC_CHSELR_CHSEL4
#define   ADC_5_CHNL                       ADC_CHSELR_CHSEL5
#define   ADC_6_CHNL                       ADC_CHSELR_CHSEL6
#define   ADC_7_CHNL                       ADC_CHSELR_CHSEL7
#define   ADC_8_CHNL                       ADC_CHSELR_CHSEL8
#define   ADC_9_CHNL                       ADC_CHSELR_CHSEL9

//ПДП (DMA)
#define   DMA_FOR_ADC                      DMA1
#define   DMA_FOR_ADC_ENR                  AHBENR
#define   DMA_FOR_ADC_CLK_EN               RCC_AHBENR_DMAEN
#define   DMA_CNL_FOR_ADC                  DMA1_Channel1
#define   DMA_CNL_FOR_ADC_IRQ              DMA1_Channel1_IRQn

//KEYs
#define   KEY_0_PIN                        0
#define   KEY_1_PIN                        1
#define   KEYS_0_1_GPIO                    GPIOF
#define   KEY_2_PIN                        3
#define   KEY_4_PIN                        4
#define   KEY_5_PIN                        5
#define   KEYS_2_4_5_GPIO                  GPIOB
#define   KEY_3_PIN                        8
#define   KEYS_3_GPIO                      GPIOA

//Delay timer
#define   DELAY_TIMER                      TIM16
#define   DELAY_TIMER_ENR                  APB2ENR
#define   DELAY_TIMER_CLK_EN               RCC_APB2ENR_TIM16EN

#if defined(TAS3251_DSP)
   #define   DELAY_TIMER_BOOT_PSC             11999//7999
   #define   DELAY_TIMER_BOOT_ARR             9999//5999
#else
   #define   DELAY_TIMER_BOOT_PSC             7999
   #define   DELAY_TIMER_BOOT_ARR             5999
#endif

#define   DELAY_TIMER_PSC                  39
#define   DELAY_TIMER_ARR                  29

#define   ADC_CHANNELS_MAX                 10
#define   KEYS_COUNT_MAX                   6

#if defined(TAS3251_DSP)
  #define   ADC_CHANNELS                     2//9
  #define   KEYS_COUNT                       1
#elif defined(TAS3251_DSPx2)
  #define   ADC_CHANNELS                     3//9
  #define   KEYS_COUNT                       1
#elif defined(ADAU1761_DSP) || defined(ADAU1701_DSP)
  #define   ADC_CHANNELS                     9//5
  #define   KEYS_COUNT                       3
  #define   SOURCE_SEL_KEY                   0
  #define   I2S_SOURCE_LEVEL                 1
  #define   ANALOG_SOURCE_LEVEL              0
#endif

#define   POTS_CHANGED_MASK                0x01FF
#define   KEYS_STATUS_OFFSET               ADC_CHANNELS_MAX
#define   KEYS_CHANGED_MASK                (0x07 << KEYS_STATUS_OFFSET)
#define   EFFECTS_STATUS_OFFSET            (ADC_CHANNELS_MAX + KEYS_COUNT_MAX)
#define   MUX_STATUS_MASK                  (1 << EFFECTS_STATUS_OFFSET)
#define   MUTE_STATUS_MASK                 (1 << (EFFECTS_STATUS_OFFSET + 1))
#define   BASS_STATUS_MASK                 (1 << (EFFECTS_STATUS_OFFSET + 2))

#define   VOLUME_MIN                       0
#define   ADC_STEP_PER_VOLUME_UNIT         140
#define   BALANCE_CENTER_LOW               1500
#define   BALANCE_CENTER_HIGH              2500
#define   BALANCE_MAX                      5
#define   ADC_STEP_PER_BALANCE_UNIT        300 //(BALANCE_CENTER_LOW / BALANCE_MAX)
#define   ADC_STEP_PER_EQ_UNIT             135

#define   EFFECT_OFF                       0
#define   EFFECT_ON                        1


typedef struct
{
  uint16_t  ParamAdr;
  uint8_t*  ParamData;
  uint16_t  ADCStep;
  void      (*Response)(uint8_t, uint16_t);
  
} CtrlPOTType;

typedef struct
{  
  void      (*Response)(uint8_t);
  
} CtrlKeyType;



void Init();

void ADCCallBack(uint16_t ADCStatus);

void KeysCallBack(uint8_t KeyNum, uint8_t KeyLevel);

void Delay();

void UpdateVolumeValue(uint16_t ADCValue);

void DSP_Init();

void DSP_ChangeVolume(uint8_t POTNum, uint16_t ADCValue);

#if !defined(TAS3251_DSPx2) 
void DSP_ChangeBalance(uint8_t POTNum, uint16_t ADCValue);

void UpdateBalanceValue(uint16_t ADCValue);
#endif

void DSP_ChangeEQParam(uint8_t POTNum, uint16_t ADCValue);

void DSP_Mux(uint8_t Flag);

void DSP_Mute(uint8_t Flag);

void DSP_Bass(uint8_t Flag);

void DSP_UpdateVolume();

void DSP_Write(uint16_t StartAddress, uint16_t BytesNum, uint8_t *DataArray);

#if defined(TAS3251_DSP)
  //void TAS3251_SwapFlag();
  //void TAS3251_WriteReg(uint8_t RegAdr, uint8_t Value);
#elif defined(TAS3251_DSPx2)
  void DSP_ChangeBalance_LR(uint8_t POTNum, uint16_t ADCValue);
  void DSP_ChangeBalance_FR(uint8_t POTNum, uint16_t ADCValue);
  void UpdateBalanceValue(uint8_t *Balance_1, uint8_t *Balance_2, uint16_t ADCValue);
#elif defined(ADAU1761_DSP)
  void ADAU1761_SafeLoad(uint16_t ParamAddr, uint8_t* ParamData, uint8_t NumLoad);
#elif defined(ADAU1701_DSP)
  void ADAU1701_SafeLoadInit(uint16_t SafeAddrAddr, uint16_t SafeDataAddr, uint16_t ParamAddr, uint8_t* ParamData);
  void ADAU1701_SafeLoadTransfer();
#endif



#endif //__BOARD_H



