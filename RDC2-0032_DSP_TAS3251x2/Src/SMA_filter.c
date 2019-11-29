/*
********************************************************************************
* COPYRIGHT(c) ЗАО «ЧИП и ДИП», 2019
* 
* Программное обеспечение предоставляется на условиях «как есть» (as is).
* При распространении указание автора обязательно.
********************************************************************************
*/


#include "SMA_filter.h"



void SMAFilter(uint16_t NewValue, SMAType *Filter)
{
  Filter->Data[SMA_SAMPLES] = NewValue / SMA_SAMPLES;
  Filter->Result = Filter->Result - Filter->Data[0] + Filter->Data[SMA_SAMPLES];
        
  for(uint8_t i = 0; i < SMA_SAMPLES; i++)
    Filter->Data[i] = Filter->Data[i + 1];
}


