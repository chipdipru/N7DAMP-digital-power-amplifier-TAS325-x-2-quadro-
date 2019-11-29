/*
********************************************************************************
* COPYRIGHT(c) ЗАО «ЧИП и ДИП», 2019
* 
* Программное обеспечение предоставляется на условиях «как есть» (as is).
* При распространении указание автора обязательно.
********************************************************************************
*/


#include "KEYs.h"


static void (*KeyLevelChanged)(uint8_t, uint8_t);

static KeyType Keys[KEYS_COUNT];


void TIM17_IRQHandler(void)
{
  KEY_SCAN_TIMER->SR = 0;
  
  for(uint8_t i = 0; i < KEYS_COUNT; i++)
  {
    if (Keys[i].State == KEY_LEVEL_CHANGE_PENDING)     
    {
      Keys[i].Counter++;
      
      if (Keys[i].Counter == KEYS_DEBOUNCE_TIME)
      {
        uint8_t NewLevel = GetKeyLevel(i);
        
        if (Keys[i].Level != NewLevel)
          KeyLevelChanged(i, NewLevel);
        
        Keys[i].State = 0;
        Keys[i].Counter = 0;
        Keys[i].Level = NewLevel;
      }      
    }
    
    else if (Keys[i].Level != GetKeyLevel(i))
      Keys[i].State = KEY_LEVEL_CHANGE_PENDING;
  }
}
//------------------------------------------------------------------------------
void Keys_Init(void (*KeyChangedCallback)(uint8_t, uint8_t))
{ 
  Keys[0].Pin = KEY_0_PIN;
  Keys[0].GPIO = KEYS_0_1_GPIO;

#if defined(ADAU1761_DSP) || defined(ADAU1701_DSP)
  Keys[1].Pin = KEY_1_PIN;
  Keys[1].GPIO = KEYS_0_1_GPIO;
  Keys[2].Pin = KEY_2_PIN;
  Keys[2].GPIO = KEYS_2_4_5_GPIO;
#endif  
      
  for(uint8_t i = 0; i < KEYS_COUNT; i++)
  {
    Keys[i].State = 0;
    Keys[i].Counter = 0;
    Keys[i].Level = GetKeyLevel(i);
  }
  
  //таймер, период 1 мс
  RCC->KEY_SCAN_TIMER_ENR |= KEY_SCAN_TIMER_CLK_EN;
  KEY_SCAN_TIMER->PSC = KEY_SCAN_TIMER_PSC;
  KEY_SCAN_TIMER->ARR = KEY_SCAN_TIMER_ARR;
  KEY_SCAN_TIMER->DIER = TIM_DIER_UIE;  
  NVIC_EnableIRQ(KEY_SCAN_TIMER_IRQ);
  NVIC_SetPriority(KEY_SCAN_TIMER_IRQ, 2);
  
  KeyLevelChanged = KeyChangedCallback;
}
//------------------------------------------------------------------------------
uint8_t GetKeyLevel(uint8_t KeyNum)
{
  return ((Keys[KeyNum].GPIO->IDR & (1 << Keys[KeyNum].Pin)) >> Keys[KeyNum].Pin);
}
//------------------------------------------------------------------------------
void Keys_Enable()
{
  KEY_SCAN_TIMER->CR1 |= TIM_CR1_CEN;
}





