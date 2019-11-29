/*
********************************************************************************
* COPYRIGHT(c) ЗАО «ЧИП и ДИП», 2019
* 
* Программное обеспечение предоставляется на условиях «как есть» (as is).
* При распространении указание автора обязательно.
********************************************************************************
*/



#ifndef __KEYS_H
#define __KEYS_H


#include "Board.h"


#define       KEYS_DEBOUNCE_TIME           70 //мс

//таймер
#define       KEY_SCAN_TIMER               TIM17
#define       KEY_SCAN_TIMER_ENR           APB2ENR
#define       KEY_SCAN_TIMER_CLK_EN        RCC_APB2ENR_TIM17EN
#define       KEY_SCAN_TIMER_IRQ           TIM17_IRQn
#define       KEY_SCAN_TIMER_PSC           239
#define       KEY_SCAN_TIMER_ARR           199

#define       KEY_LEVEL_CHANGE_PENDING     (1 << 0)
#define       KEY_PRESSED_LEVEL            0


typedef struct
{
  volatile uint8_t  State;
  uint8_t           Pin;
  volatile uint8_t  Level;
  volatile uint16_t Counter;
  GPIO_TypeDef*     GPIO;
  
} KeyType;


void TIM17_IRQHandler(void);

void Keys_Init(void (*KeyChangedCallback)(uint8_t, uint8_t));

uint8_t GetKeyLevel(uint8_t KeyNum);

void Keys_Enable();


#endif //__KEYS_H


