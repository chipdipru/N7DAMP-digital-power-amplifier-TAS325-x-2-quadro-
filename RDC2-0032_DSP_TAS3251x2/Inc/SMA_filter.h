/*
********************************************************************************
* COPYRIGHT(c) ��� ���� � ��ϻ, 2019
* 
* ����������� ����������� ��������������� �� �������� ���� ����� (as is).
* ��� ��������������� �������� ������ �����������.
********************************************************************************
*/


#ifndef __SMA_FILTER_H
#define __SMA_FILTER_H


#include "stdint.h"



#define       SMA_SAMPLES                  4



typedef struct
{
  uint16_t Data[SMA_SAMPLES + 1];
  uint16_t Result;
    
} SMAType;


void SMAFilter(uint16_t NewValue, SMAType *Filter);


#endif //__SMA_FILTER_H