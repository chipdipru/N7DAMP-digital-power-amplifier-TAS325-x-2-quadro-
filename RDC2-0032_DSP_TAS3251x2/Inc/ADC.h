/*
********************************************************************************
* COPYRIGHT(c) ЗАО «ЧИП и ДИП», 2019
* 
* Программное обеспечение предоставляется на условиях «как есть» (as is).
* При распространении указание автора обязательно.
********************************************************************************
*/


#ifndef __ADC_H
#define __ADC_H


#include "Board.h"
#include "SMA_filter.h"


#define   ADC_MAX_VALUE                    4095

#define   ADC_TIMER                        TIM14
#define   ADC_TIMER_ENR                    APB1ENR
#define   ADC_TIMER_CLK_EN                 RCC_APB1ENR_TIM14EN
#define   ADC_TIMER_IRQ                    TIM14_IRQn
//10 ms
#define   ADC_TIMER_PSC                    799
#define   ADC_TIMER_ARR                    599

#define   ADC_THRESHOLD                    50



typedef struct
{
  volatile uint16_t CurValue;
  SMAType           Filter;  
  
} ADCType;


void ADC_Init(void (*ADCValuesChangedCallback)(uint16_t));

void ADC_Start();

void TIM14_IRQHandler(void);

uint16_t ADC_GetValue(uint8_t ADCChnl);



#endif //__ADC_H

