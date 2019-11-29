/*
********************************************************************************
* COPYRIGHT(c) ЗАО «ЧИП и ДИП», 2019
* 
* Программное обеспечение предоставляется на условиях «как есть» (as is).
* При распространении указание автора обязательно.
********************************************************************************
*/


#ifndef __ADAU1761_H
#define __ADAU1761_H


#define   ADAU1761_I2C_ADDRESS             0x38


#define   ADAU1761_MUX_1_0_ADR             8
#define   ADAU1761_MUX_1_1_ADR             9
#define   ADAU1761_MUTE_ADR                12
#define   ADAU1761_EQ125Hz_START_ADR       14
#define   ADAU1761_EQ500Hz_START_ADR       19
#define   ADAU1761_EQ1080Hz_START_ADR      24
#define   ADAU1761_EQ2000Hz_START_ADR      29
#define   ADAU1761_EQ4000Hz_START_ADR      34
#define   ADAU1761_EQ7800Hz_START_ADR      39
#define   ADAU1761_EQ9800Hz_START_ADR      44
#define   ADAU1761_BASS_ENABLE_ADR         89
#define   ADAU1761_LEFT_VOL_ADR            104
#define   ADAU1761_LEFT_VOL_STEP_ADR       105
#define   ADAU1761_RIGHT_VOL_ADR           106
#define   ADAU1761_RIGHT_VOL_STEP_ADR      107

#define   ADAU1761_SAFELOAD_DATA_START_ADR 0x01
#define   ADAU1761_SAFELOAD_PARAM_ADR      0x06
#define   ADAU1761_SAFELOAD_NUMLOAD        0x07

#define   ADAU1761_SAFELOAD_NUMLOAD_1      1
#define   ADAU1761_SAFELOAD_NUMLOAD_5      5

#define   ADAU1761_VOLUME_MUTE             40


#endif //__ADAU1761_H
