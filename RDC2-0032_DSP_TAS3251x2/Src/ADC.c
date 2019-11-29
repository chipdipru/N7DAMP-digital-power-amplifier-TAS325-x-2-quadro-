/*
********************************************************************************
* COPYRIGHT(c) ЗАО «ЧИП и ДИП», 2019
* 
* Программное обеспечение предоставляется на условиях «как есть» (as is).
* При распространении указание автора обязательно.
********************************************************************************
*/




#include "ADC.h"


static void (*ADCValuesChanged)(uint16_t);
static ADCType ADCs[ADC_CHANNELS];
static uint16_t ADCRawValues[ADC_CHANNELS] = {0};



void ADC_Init(void (*ADCValuesChangedCallback)(uint16_t))
{
  ADCValuesChanged = ADCValuesChangedCallback;
  
  ADC_GPIO->MODER |= (3 << (2 * ADC_0_PIN)) | (3 << (2 * ADC_1_PIN)) | (3 << (2 * ADC_2_PIN)) | \
                     (3 << (2 * ADC_3_PIN)) | (3 << (2 * ADC_4_PIN)) | (3 << (2 * ADC_5_PIN)) | \
                     (3 << (2 * ADC_6_PIN)) | (3 << (2 * ADC_7_PIN));
  
  ADC_GPIO2->MODER |= (3 << (2 * ADC_8_PIN));// | (3 << (2 * ADC_9_PIN));
  
  RCC->ADC_ENR |= ADC_CLK_EN;
  
  ADC_USED->CR = ADC_CR_ADCAL;
  while(((ADC_USED->CR) & ADC_CR_ADCAL) == ADC_CR_ADCAL);
  
  ADC_USED->CFGR2 = ADC_CFGR2_CKMODE_1;
  ADC_USED->SMPR = ADC_SMPR_SMP_2 | ADC_SMPR_SMP_1 | ADC_SMPR_SMP_0;

#if defined(TAS3251_DSP)
  ADC_USED->CHSELR = ADC_0_CHNL | ADC_1_CHNL;// | ADC_2_CHNL | ADC_3_CHNL | ADC_4_CHNL | \
                     ADC_5_CHNL | ADC_6_CHNL | ADC_7_CHNL | ADC_8_CHNL | ADC_9_CHNL;
#elif defined(TAS3251_DSPx2)
  ADC_USED->CHSELR = ADC_0_CHNL | ADC_1_CHNL | ADC_2_CHNL;// | ADC_3_CHNL | ADC_4_CHNL | \
                     ADC_5_CHNL | ADC_6_CHNL | ADC_7_CHNL | ADC_8_CHNL | ADC_9_CHNL;
#elif defined(ADAU1761_DSP) || defined(ADAU1701_DSP)
  ADC_USED->CHSELR = ADC_0_CHNL | ADC_1_CHNL | ADC_2_CHNL | ADC_3_CHNL | ADC_4_CHNL | \
                     ADC_5_CHNL | ADC_6_CHNL | ADC_7_CHNL | ADC_8_CHNL;// | ADC_9_CHNL;
#endif  
  
  ADC_USED->CFGR1 = ADC_CFGR1_DMAEN | ADC_CFGR1_DMACFG | ADC_CFGR1_CONT;
  ADC_USED->CR |= ADC_CR_ADEN;
  while(((ADC_USED->ISR) & ADC_ISR_ADRDY) != ADC_ISR_ADRDY);
  
  RCC->DMA_FOR_ADC_ENR |= DMA_FOR_ADC_CLK_EN;
  DMA_CNL_FOR_ADC->CPAR = (uint32_t)(&(ADC_USED->DR));
  DMA_CNL_FOR_ADC->CMAR = (uint32_t)(ADCRawValues);
  DMA_CNL_FOR_ADC->CNDTR = ADC_CHANNELS;
  DMA_CNL_FOR_ADC->CCR |= DMA_CCR_MINC | DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0 | DMA_CCR_CIRC;
  DMA_CNL_FOR_ADC->CCR |= DMA_CCR_EN;
  
  for (uint8_t chnl = 0; chnl < ADC_CHANNELS; chnl++)
  {
    ADCs[chnl].CurValue = ADC_MAX_VALUE;
    
    ADCs[chnl].Filter.Result = 0;
        
    for (uint8_t i = 0; i < SMA_SAMPLES; i++)
      ADCs[chnl].Filter.Data[i] = 0;
  }
  
  RCC->ADC_TIMER_ENR |= ADC_TIMER_CLK_EN;
  ADC_TIMER->PSC = ADC_TIMER_PSC;
  ADC_TIMER->ARR = ADC_TIMER_ARR;
  ADC_TIMER->EGR = TIM_EGR_UG;
  NVIC_EnableIRQ(ADC_TIMER_IRQ);
  NVIC_SetPriority(ADC_TIMER_IRQ, 1);
  ADC_TIMER->SR = 0;
  ADC_TIMER->DIER = TIM_DIER_UIE;
}
//------------------------------------------------------------------------------
void ADC_Start()
{
  ADC_USED->CR |= ADC_CR_ADSTART;
  ADC_TIMER->CR1 |= TIM_CR1_CEN;
}
//------------------------------------------------------------------------------
void TIM14_IRQHandler(void)
{
  ADC_TIMER->SR = 0;
  
  uint16_t ADCStatus = 0;
  
  for (uint8_t chnl = 0; chnl < ADC_CHANNELS; chnl++)
  {
    SMAFilter(ADCRawValues[chnl], &ADCs[chnl].Filter);
    
    if (((ADCs[chnl].CurValue > ADCs[chnl].Filter.Result) && ((ADCs[chnl].CurValue - ADCs[chnl].Filter.Result) >= ADC_THRESHOLD))
     || ((ADCs[chnl].CurValue < ADCs[chnl].Filter.Result) && ((ADCs[chnl].Filter.Result - ADCs[chnl].CurValue) >= ADC_THRESHOLD)))
    {
      ADCs[chnl].CurValue = ADCs[chnl].Filter.Result;
      ADCStatus |= 1 << chnl;
    }
  }
  
  if (ADCStatus != 0)
    ADCValuesChanged(ADCStatus);
}
//------------------------------------------------------------------------------
uint16_t ADC_GetValue(uint8_t ADCChnl)
{
  return ADCs[ADCChnl].CurValue;
}


