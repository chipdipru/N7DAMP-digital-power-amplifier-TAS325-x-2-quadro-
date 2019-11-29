/*
********************************************************************************
* COPYRIGHT(c) ЗАО «ЧИП и ДИП», 2019
* 
* Программное обеспечение предоставляется на условиях «как есть» (as is).
* При распространении указание автора обязательно.
********************************************************************************
*/


#ifndef __ADAU1701_H
#define __ADAU1701_H


#define   ADAU1701_I2C_ADDRESS             0x34

#define   ADAU1701_MUX_1_0_ADR             0
#define   ADAU1701_MUX_1_1_ADR             1
#define   ADAU1701_MUTE_ADR                4
#define   ADAU1701_EQ125Hz_START_ADR       5
#define   ADAU1701_EQ500Hz_START_ADR       10
#define   ADAU1701_EQ1080Hz_START_ADR      15
#define   ADAU1701_EQ2000Hz_START_ADR      20
#define   ADAU1701_EQ4000Hz_START_ADR      25
#define   ADAU1701_EQ7800Hz_START_ADR      30
#define   ADAU1701_EQ9800Hz_START_ADR      35
#define   ADAU1701_BASS_ENABLE_ADR         80
#define   ADAU1701_LEFT_VOL_ADR            95
#define   ADAU1701_LEFT_VOL_STEP_ADR       96
#define   ADAU1701_RIGHT_VOL_ADR           97
#define   ADAU1701_RIGHT_VOL_STEP_ADR      98


#define   ADAU1701_DSP_CORE_CTRL           0x081C
#define   ADAU1701_DAC_SETUP               0x0827
#define   ADAU1701_SAFELOAD_ADR_0          0x0815
#define   ADAU1701_SAFELOAD_DATA_0         0x0810

#define   ADAU1701_DSP_CORE_IST            (1 << 5)


#endif //__ADAU1701_H
