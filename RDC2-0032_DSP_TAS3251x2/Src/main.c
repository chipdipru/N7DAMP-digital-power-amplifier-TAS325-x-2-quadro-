/*
********************************************************************************
* COPYRIGHT(c) ЗАО «ЧИП и ДИП», 2019
* 
* Программное обеспечение предоставляется на условиях «как есть» (as is).
* При распространении указание автора обязательно.
********************************************************************************
*/


#include "Board.h"
#include "I2C.h"
#include "ADC.h"
#include "KEYs.h"
#include "TAS3251.h"

#if defined(ADAU1761_DSP)
  #include "ADAU1761.h"
  #include "ADAU1761_RDC2-0032_IC_1.h"
  #include "ADAU1761_RDC2-0032_IC_1_REG.h"
  #include "ADAU_PARAM_DATA.h"
#endif

#if defined(ADAU1701_DSP)
  #include "ADAU1701.h"
  #include "ADAU1701_RDC2-0032_IC_1.h"
  #include "ADAU1701_RDC2-0032_IC_1_REG.h"
  #include "ADAU_PARAM_DATA.h"
#endif


static uint32_t Status = 0;
static uint8_t MainVolume = VOLUME_MIN;
static uint8_t BalanceLeft;
static uint8_t BalanceRight;
#if defined(TAS3251_DSPx2) 
static uint8_t BalanceLeft_2;
static uint8_t BalanceRight_2;
static uint8_t BalanceSide_Left = 0;
static uint8_t BalanceSide_Right = 0;
static uint8_t BalanceSide_Front = 0;
static uint8_t BalanceSide_Rear = 0;
#endif
static CtrlPOTType UserPOTs[ADC_CHANNELS];
static CtrlKeyType UserKeys[KEYS_COUNT];


int main()
{
  Init();
  
  for(;;)
  {
    __WFI();
    
    if ((Status & POTS_CHANGED_MASK) != 0)
    {
      for (uint8_t i = 0; i < ADC_CHANNELS; i++)
      {
        uint16_t ActivePOT = (1 << i);
        if ((Status & ActivePOT) != 0)
        {
          UserPOTs[i].Response(i, ADC_GetValue(i));
          Status &= ~ActivePOT;
        }
      }
      
      if ((Status & MUTE_STATUS_MASK) == MUTE_STATUS_MASK)
      {
        DSP_Mute(EFFECT_OFF);
        Status &= ~MUTE_STATUS_MASK;
      }
    }
    
    if ((Status & KEYS_CHANGED_MASK) != 0)
    {
      for (uint8_t i = 0; i < KEYS_COUNT; i++)
      {
        uint16_t ActiveKey = (1 << i) << KEYS_STATUS_OFFSET;
        if ((Status & ActiveKey) != 0)
        {
          uint8_t NewEffectStatus;
          uint8_t EffectStatusOffset = EFFECTS_STATUS_OFFSET + i;
          if ((Status & (1 << EffectStatusOffset)) == 0)
          {
            Status |= (1 << EffectStatusOffset);
            NewEffectStatus = EFFECT_ON;
          }
          else
          {
            Status &= ~(1 << EffectStatusOffset);
            NewEffectStatus = EFFECT_OFF;
          }
          
          UserKeys[i].Response(NewEffectStatus);
          Status &= ~ActiveKey;
        }
      }
    }
  }
}
//------------------------------------------------------------------------------
void Init()
{
  //HSI, PLL, 48 MHz
  FLASH->ACR = FLASH_ACR_PRFTBE | (uint32_t)FLASH_ACR_LATENCY;
  RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSI_DIV2 | RCC_CFGR_PLLMUL12);
  RCC->CR |= RCC_CR_PLLON;
  while((RCC->CR & RCC_CR_PLLRDY) == 0);
 
  RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;
  while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)RCC_CFGR_SWS_PLL);
  
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN  | RCC_AHBENR_GPIOFEN; 
  
  UserPOTs[0].ADCStep = ADC_STEP_PER_VOLUME_UNIT;
  UserPOTs[0].Response = DSP_ChangeVolume;
#if !defined(TAS3251_DSPx2)  
  UserPOTs[1].ADCStep = ADC_STEP_PER_BALANCE_UNIT;
  UserPOTs[1].Response = DSP_ChangeBalance;
#endif  
  
#if defined(TAS3251_DSP)
  I2C_Init(TAS3251_I2C_ADDRESS);
  
  UserPOTs[0].ParamAdr = TAS3251_LEFT_VOLUME_REG;
  UserPOTs[0].ParamData = (uint8_t*)TAS3251_VOLUME;
  
  UserPOTs[1].ParamAdr = TAS3251_LEFT_VOLUME_REG;
  UserPOTs[1].ParamData = (uint8_t*)TAS3251_VOLUME;
  
  UserKeys[0].Response = DSP_Mute;

#elif defined(TAS3251_DSPx2)
  I2C_Init(TAS3251_I2C_ADDRESS);
  
  UserPOTs[1].ADCStep = ADC_STEP_PER_BALANCE_UNIT;
  UserPOTs[1].Response = DSP_ChangeBalance_LR;
  
  UserPOTs[2].ADCStep = ADC_STEP_PER_BALANCE_UNIT;
  UserPOTs[2].Response = DSP_ChangeBalance_FR;
  
  UserPOTs[0].ParamAdr = TAS3251_LEFT_VOLUME_REG;
  UserPOTs[0].ParamData = (uint8_t*)TAS3251_VOLUME;
  
  UserPOTs[1].ParamAdr = TAS3251_LEFT_VOLUME_REG;
  UserPOTs[1].ParamData = (uint8_t*)TAS3251_VOLUME;
  
  UserPOTs[2].ParamAdr = TAS3251_LEFT_VOLUME_REG;
  UserPOTs[2].ParamData = (uint8_t*)TAS3251_VOLUME;
  
  UserKeys[0].Response = DSP_Mute;
  
#elif defined(ADAU1761_DSP) || defined(ADAU1701_DSP)
  UserPOTs[0].ParamData = (uint8_t*)ADAU_VOLUME;
  UserPOTs[1].ParamData = (uint8_t*)ADAU_VOLUME;
  UserPOTs[2].ParamData = (uint8_t*)ADAU_EQ_125HZ;
  UserPOTs[3].ParamData = (uint8_t*)ADAU_EQ_500HZ;
  UserPOTs[4].ParamData = (uint8_t*)ADAU_EQ_1080HZ;
  UserPOTs[5].ParamData = (uint8_t*)ADAU_EQ_2000HZ;
  UserPOTs[6].ParamData = (uint8_t*)ADAU_EQ_4000HZ;
  UserPOTs[7].ParamData = (uint8_t*)ADAU_EQ_7800HZ;
  UserPOTs[8].ParamData = (uint8_t*)ADAU_EQ_9800HZ;
  
  for (uint8_t i = 2; i < ADC_CHANNELS; i++)
  {
    UserPOTs[i].ADCStep = ADC_STEP_PER_EQ_UNIT;
    UserPOTs[i].Response = DSP_ChangeEQParam;
  }
  
  UserKeys[0].Response = DSP_Mux;
  UserKeys[1].Response = DSP_Mute;
  UserKeys[2].Response = DSP_Bass;
#if defined(ADAU1761_DSP)
  I2C_Init(ADAU1761_I2C_ADDRESS);
  UserPOTs[0].ParamAdr = ADAU1761_LEFT_VOL_ADR;
  UserPOTs[1].ParamAdr = ADAU1761_LEFT_VOL_ADR;
  UserPOTs[2].ParamAdr = ADAU1761_EQ125Hz_START_ADR;
  UserPOTs[3].ParamAdr = ADAU1761_EQ500Hz_START_ADR;
  UserPOTs[4].ParamAdr = ADAU1761_EQ1080Hz_START_ADR;
  UserPOTs[5].ParamAdr = ADAU1761_EQ2000Hz_START_ADR;
  UserPOTs[6].ParamAdr = ADAU1761_EQ4000Hz_START_ADR;
  UserPOTs[7].ParamAdr = ADAU1761_EQ7800Hz_START_ADR;
  UserPOTs[8].ParamAdr = ADAU1761_EQ9800Hz_START_ADR;
#elif defined(ADAU1701_DSP)
  I2C_Init(ADAU1701_I2C_ADDRESS);
  UserPOTs[0].ParamAdr = ADAU1701_LEFT_VOL_ADR;
  UserPOTs[1].ParamAdr = ADAU1701_LEFT_VOL_ADR;
  UserPOTs[2].ParamAdr = ADAU1701_EQ125Hz_START_ADR;
  UserPOTs[3].ParamAdr = ADAU1701_EQ500Hz_START_ADR;
  UserPOTs[4].ParamAdr = ADAU1701_EQ1080Hz_START_ADR;
  UserPOTs[5].ParamAdr = ADAU1701_EQ2000Hz_START_ADR;
  UserPOTs[6].ParamAdr = ADAU1701_EQ4000Hz_START_ADR;
  UserPOTs[7].ParamAdr = ADAU1701_EQ7800Hz_START_ADR;
  UserPOTs[8].ParamAdr = ADAU1701_EQ9800Hz_START_ADR;
#endif    
#endif
  
  ADC_Init(ADCCallBack);
  Keys_Init(KeysCallBack);
  
  RCC->DELAY_TIMER_ENR |= DELAY_TIMER_CLK_EN;
  DELAY_TIMER->PSC = DELAY_TIMER_BOOT_PSC;
  DELAY_TIMER->ARR = DELAY_TIMER_BOOT_ARR;
  DELAY_TIMER->EGR = TIM_EGR_UG;  
  DELAY_TIMER->SR = 0;
  DELAY_TIMER->CR1 = TIM_CR1_OPM;
  
  Delay();
  
  RCC->DELAY_TIMER_ENR |= DELAY_TIMER_CLK_EN;
  DELAY_TIMER->PSC = DELAY_TIMER_PSC;
  DELAY_TIMER->ARR = DELAY_TIMER_ARR;
  DELAY_TIMER->EGR = TIM_EGR_UG;  
  DELAY_TIMER->SR = 0;
  RCC->DELAY_TIMER_ENR &= ~DELAY_TIMER_CLK_EN;
  
  DSP_Init();
  
#if defined(ADAU1761_DSP) || defined(ADAU1701_DSP)  
  if (GetKeyLevel(SOURCE_SEL_KEY) == ANALOG_SOURCE_LEVEL)
    DSP_Mux(EFFECT_OFF);
  else
    Status |= (1 << (EFFECTS_STATUS_OFFSET + SOURCE_SEL_KEY));
#endif
  
  ADC_Start();
  Keys_Enable();
}
//------------------------------------------------------------------------------
void ADCCallBack(uint16_t ADCStatus)
{
  Status |= ADCStatus;
}
//------------------------------------------------------------------------------
void KeysCallBack(uint8_t KeyNum, uint8_t KeyLevel)
{
#if defined(TAS3251_DSP) || defined(TAS3251_DSPx2)
  if (KeyLevel == KEY_PRESSED_LEVEL)
    Status |= (1 << KeyNum) << KEYS_STATUS_OFFSET;
#elif defined(ADAU1761_DSP) || defined(ADAU1701_DSP)
  if (KeyNum != SOURCE_SEL_KEY)
  {
    if (KeyLevel == KEY_PRESSED_LEVEL)
      Status |= (1 << KeyNum) << KEYS_STATUS_OFFSET;
  }
  
  else
    Status |= (1 << KeyNum) << KEYS_STATUS_OFFSET;
#endif
}
//------------------------------------------------------------------------------
void Delay()
{
  RCC->DELAY_TIMER_ENR |= DELAY_TIMER_CLK_EN;
  DELAY_TIMER->CR1 |= TIM_CR1_CEN;
  while((DELAY_TIMER->SR & TIM_SR_UIF) != TIM_SR_UIF);
  DELAY_TIMER->SR = 0;
  RCC->DELAY_TIMER_ENR &= ~DELAY_TIMER_CLK_EN;
}
//------------------------------------------------------------------------------
void DSP_ChangeVolume(uint8_t POTNum, uint16_t ADCValue)
{
  UpdateVolumeValue(ADCValue);
  DSP_UpdateVolume();
}
//------------------------------------------------------------------------------
#if !defined(TAS3251_DSPx2) 

void DSP_ChangeBalance(uint8_t POTNum, uint16_t ADCValue)
{
  UpdateBalanceValue(ADCValue);
  DSP_UpdateVolume();
}
//------------------------------------------------------------------------------
void UpdateBalanceValue(uint16_t ADCValue)
{        
  if ((ADCValue >= BALANCE_CENTER_LOW) && (ADCValue <= BALANCE_CENTER_HIGH))
  {
    BalanceLeft = BALANCE_MAX;
    BalanceRight = BALANCE_MAX;
  }
  else if (ADCValue < BALANCE_CENTER_LOW)
  {
    BalanceLeft = ADCValue / ADC_STEP_PER_BALANCE_UNIT;
    BalanceRight = BALANCE_MAX;
  }
  else //if (ADCValue > BALANCE_CENTER_HIGH)
  {
    BalanceLeft = BALANCE_MAX;
    BalanceRight = (ADC_MAX_VALUE - ADCValue) / ADC_STEP_PER_BALANCE_UNIT;          
  }
}

#endif
//------------------------------------------------------------------------------
void DSP_Write(uint16_t StartAddress, uint16_t BytesNum, uint8_t *DataArray)
{
  I2C_Write(StartAddress, BytesNum, DataArray);
  Delay();
}
//------------------------------------------------------------------------------
#if defined(TAS3251_DSP) || defined(TAS3251_DSPx2)

void DSP_Init()
{
  uint8_t InitData = 0;
  DSP_Write(TAS3251_CHANGE_PAGE_REG, 1, &InitData);
  DSP_Write(TAS3251_CHANGE_BOOK_REG, 1, &InitData);
  DSP_Write(TAS3251_OPERATION_MODES_REG, 1, &InitData);
  //32 bit
  InitData = 3;
  DSP_Write(0x28, 1, &InitData);
  
#if defined(TAS3251_DSPx2)
  I2C_ChangeGPIOs(I2C_TAS3251_2);
  
  InitData = 0;
  DSP_Write(TAS3251_CHANGE_PAGE_REG, 1, &InitData);
  DSP_Write(TAS3251_CHANGE_BOOK_REG, 1, &InitData);
  DSP_Write(TAS3251_OPERATION_MODES_REG, 1, &InitData);
  //32 bit
  InitData = 3;
  DSP_Write(0x28, 1, &InitData);
  
  I2C_ChangeGPIOs(I2C_TAS3251_1);
#endif
    
/*  
  uint16_t DataSize = sizeof(TAS3251_INIT_DATA) / 2;
  
  for (uint16_t i = 0; i < DataSize; i++)
    DSP_Write(TAS3251_INIT_DATA[i][0], 1, (uint8_t*)(&TAS3251_INIT_DATA[i][1]));
*/  
}
//------------------------------------------------------------------------------
void UpdateVolumeValue(uint16_t ADCValue)
{
  //ADCValue = ADC_MAX_VALUE - ADCValue;
  MainVolume = (ADCValue / ADC_STEP_PER_VOLUME_UNIT);
}
//------------------------------------------------------------------------------
void DSP_UpdateVolume()
{
  uint8_t LeftVolume = MainVolume * BalanceLeft / BALANCE_MAX;
  uint8_t RightVolume = MainVolume * BalanceRight / BALANCE_MAX;
  
#if defined(TAS3251_DSPx2)
  
  BalanceLeft = (BalanceSide_Left + BalanceSide_Front) / 2;
  BalanceRight = (BalanceSide_Right + BalanceSide_Front) / 2;
  BalanceLeft_2 = (BalanceSide_Left + BalanceSide_Rear) / 2;
  BalanceRight_2 = (BalanceSide_Right + BalanceSide_Rear) / 2;
  
  LeftVolume = MainVolume * BalanceLeft / BALANCE_MAX;
  RightVolume = MainVolume * BalanceRight / BALANCE_MAX;
  
  uint8_t LeftVolume_2 = MainVolume * BalanceLeft_2 / BALANCE_MAX;
  uint8_t RightVolume_2 = MainVolume * BalanceRight_2 / BALANCE_MAX;
#endif
  
  uint8_t ChnlVolume[] = { TAS3251_VOLUME[LeftVolume], TAS3251_VOLUME[RightVolume], };
  DSP_Write(TAS3251_LEFT_VOLUME_REG, 2, ChnlVolume);

#if defined(TAS3251_DSPx2)
  I2C_ChangeGPIOs(I2C_TAS3251_2);
  
  ChnlVolume[0] = TAS3251_VOLUME[LeftVolume_2];
  ChnlVolume[1] = TAS3251_VOLUME[RightVolume_2];
  DSP_Write(TAS3251_LEFT_VOLUME_REG, 2, ChnlVolume);
  
  I2C_ChangeGPIOs(I2C_TAS3251_1);
#endif
}
//------------------------------------------------------------------------------
void DSP_Mute(uint8_t Flag)
{
  if (Flag == EFFECT_ON)
  {
    uint8_t ChnlVolume[] = { TAS3251_VOLUME[TAS3251_VOL_MUTE], TAS3251_VOLUME[TAS3251_VOL_MUTE], };
    DSP_Write(TAS3251_LEFT_VOLUME_REG, 2, ChnlVolume);
    
#if defined(TAS3251_DSPx2)
  I2C_ChangeGPIOs(I2C_TAS3251_2);
  DSP_Write(TAS3251_LEFT_VOLUME_REG, 2, ChnlVolume);
  I2C_ChangeGPIOs(I2C_TAS3251_1);
#endif
  }
  else
    DSP_UpdateVolume();
}
//------------------------------------------------------------------------------
#if defined(TAS3251_DSPx2)

void DSP_ChangeBalance_LR(uint8_t POTNum, uint16_t ADCValue)
{
  UpdateBalanceValue(&BalanceSide_Left, &BalanceSide_Right, ADCValue);
  DSP_UpdateVolume();
}
//------------------------------------------------------------------------------
void DSP_ChangeBalance_FR(uint8_t POTNum, uint16_t ADCValue)
{
  UpdateBalanceValue(&BalanceSide_Front, &BalanceSide_Rear, ADCValue);
  DSP_UpdateVolume();
}
//------------------------------------------------------------------------------
void UpdateBalanceValue(uint8_t *Balance_1, uint8_t *Balance_2, uint16_t ADCValue)
{        
  if ((ADCValue >= BALANCE_CENTER_LOW) && (ADCValue <= BALANCE_CENTER_HIGH))
  {
    *Balance_1 = BALANCE_MAX;
    *Balance_2 = BALANCE_MAX;
  }
  else if (ADCValue < BALANCE_CENTER_LOW)
  {
    *Balance_1 = ADCValue / ADC_STEP_PER_BALANCE_UNIT;
    *Balance_2 = BALANCE_MAX;
  }
  else //if (ADCValue > BALANCE_CENTER_HIGH)
  {
    *Balance_1 = BALANCE_MAX;
    *Balance_2 = (ADC_MAX_VALUE - ADCValue) / ADC_STEP_PER_BALANCE_UNIT;
  }
}

#endif //TAS3251_DSPx2

#elif defined(ADAU1761_DSP)

void DSP_Init()
{
  DSP_Write( REG_SAMPLE_RATE_SETTING_IC_1_ADDR, REG_SAMPLE_RATE_SETTING_IC_1_BYTE, (uint8_t *)R0_SAMPLE_RATE_SETTING_IC_1_Default );
  DSP_Write( REG_DSP_RUN_REGISTER_IC_1_ADDR, REG_DSP_RUN_REGISTER_IC_1_BYTE, (uint8_t *)R1_DSP_RUN_REGISTER_IC_1_Default );
  DSP_Write( REG_CLKCTRLREGISTER_IC_1_ADDR, REG_CLKCTRLREGISTER_IC_1_BYTE, (uint8_t *)R2_CLKCTRLREGISTER_IC_1_Default );
  DSP_Write( REG_PLLCRLREGISTER_IC_1_ADDR, REG_PLLCRLREGISTER_IC_1_BYTE, (uint8_t *)R3_PLLCRLREGISTER_IC_1_Default );
  //delay
  DSP_Write( 0x0000, R4_DELAY_IC_1_SIZE, (uint8_t *)R4_DELAY_IC_1_Default );
  
  DSP_Write( REG_SERIAL_PORT_CONTROL_0_IC_1_ADDR , R5_SERIAL_PORT_CONTROL_REGISTERS_IC_1_SIZE, (uint8_t *)R5_SERIAL_PORT_CONTROL_REGISTERS_IC_1_Default );
  DSP_Write( REG_ALC_CONTROL_0_IC_1_ADDR , R6_ALC_CONTROL_REGISTERS_IC_1_SIZE, (uint8_t *)R6_ALC_CONTROL_REGISTERS_IC_1_Default );
  DSP_Write( REG_MICCTRLREGISTER_IC_1_ADDR, REG_MICCTRLREGISTER_IC_1_BYTE, (uint8_t *)R7_MICCTRLREGISTER_IC_1_Default );
  DSP_Write( REG_RECORD_PWR_MANAGEMENT_IC_1_ADDR , R8_RECORD_INPUT_SIGNAL_PATH_REGISTERS_IC_1_SIZE, (uint8_t *)R8_RECORD_INPUT_SIGNAL_PATH_REGISTERS_IC_1_Default );
  DSP_Write( REG_ADC_CONTROL_0_IC_1_ADDR , R9_ADC_CONTROL_REGISTERS_IC_1_SIZE, (uint8_t *)R9_ADC_CONTROL_REGISTERS_IC_1_Default );
  DSP_Write( REG_PLAYBACK_MIXER_LEFT_CONTROL_0_IC_1_ADDR , R10_PLAYBACK_OUTPUT_SIGNAL_PATH_REGISTERS_IC_1_SIZE, (uint8_t *)R10_PLAYBACK_OUTPUT_SIGNAL_PATH_REGISTERS_IC_1_Default );
  DSP_Write( REG_CONVERTER_CTRL_0_IC_1_ADDR , R11_CONVERTER_CONTROL_REGISTERS_IC_1_SIZE, (uint8_t *)R11_CONVERTER_CONTROL_REGISTERS_IC_1_Default );
  DSP_Write( REG_DAC_CONTROL_0_IC_1_ADDR , R12_DAC_CONTROL_REGISTERS_IC_1_SIZE, (uint8_t *)R12_DAC_CONTROL_REGISTERS_IC_1_Default );
  DSP_Write( REG_SERIAL_PORT_PAD_CONTROL_0_IC_1_ADDR , R13_SERIAL_PORT_PAD_CONTROL_REGISTERS_IC_1_SIZE, (uint8_t *)R13_SERIAL_PORT_PAD_CONTROL_REGISTERS_IC_1_Default );
  DSP_Write( REG_COMM_PORT_PAD_CTRL_0_IC_1_ADDR , R14_COMMUNICATION_PORT_PAD_CONTROL_REGISTERS_IC_1_SIZE, (uint8_t *)R14_COMMUNICATION_PORT_PAD_CONTROL_REGISTERS_IC_1_Default );
  DSP_Write( REG_JACKREGISTER_IC_1_ADDR, REG_JACKREGISTER_IC_1_BYTE, (uint8_t *)R15_JACKREGISTER_IC_1_Default );
  
  //clear
  for (uint16_t i = 0; i < 204; i++)
  {
    I2C_ADAU_CLEAR_REGISTER_BLOCK( 0x0800 + i, 5 );
    Delay();
  }
  for (uint16_t i = 0; i < 204; i++)
  {
    I2C_ADAU_CLEAR_REGISTER_BLOCK( 0x08CC + i, 5 );
    Delay();
  }
  for (uint16_t i = 0; i < 204; i++)
  {
    I2C_ADAU_CLEAR_REGISTER_BLOCK( 0x0998 + i, 5 );
    Delay();
  }
  for (uint16_t i = 0; i < 204; i++)
  {
    I2C_ADAU_CLEAR_REGISTER_BLOCK( 0x0A64 + i, 5 );
    Delay();
  }
  
  for (uint16_t i = 0; i < 3; i++)
  {
    I2C_ADAU_CLEAR_REGISTER_BLOCK( 0x0BFC + i, 5 );
    Delay();
  }
  
  DSP_Write( REG_DSP_ENABLE_REGISTER_IC_1_ADDR, REG_DSP_ENABLE_REGISTER_IC_1_BYTE, (uint8_t *)R21_DSP_ENABLE_REGISTER_IC_1_Default );
  DSP_Write( REG_CRC_IDEAL_1_IC_1_ADDR , R22_CRC_REGISTERS_IC_1_SIZE, (uint8_t *)R22_CRC_REGISTERS_IC_1_Default );
  DSP_Write( REG_GPIO_0_CONTROL_IC_1_ADDR , R23_GPIO_REGISTERS_IC_1_SIZE, (uint8_t *)R23_GPIO_REGISTERS_IC_1_Default );
  DSP_Write( REG_NON_MODULO_RAM_1_IC_1_ADDR , R24_NON_MODULO_REGISTERS_IC_1_SIZE, (uint8_t *)R24_NON_MODULO_REGISTERS_IC_1_Default );
  DSP_Write( REG_WATCHDOG_ENABLE_IC_1_ADDR , R25_WATCHDOG_REGISTERS_IC_1_SIZE, (uint8_t *)R25_WATCHDOG_REGISTERS_IC_1_Default );
  DSP_Write( REG_SAMPLE_RATE_SETTING_IC_1_ADDR, REG_SAMPLE_RATE_SETTING_IC_1_BYTE, (uint8_t *)R26_SAMPLE_RATE_SETTING_IC_1_Default );
  DSP_Write( REG_ROUTING_MATRIX_INPUTS_IC_1_ADDR, REG_ROUTING_MATRIX_INPUTS_IC_1_BYTE, (uint8_t *)R27_ROUTING_MATRIX_INPUTS_IC_1_Default );
  DSP_Write( REG_ROUTING_MATRIX_OUTPUTS_IC_1_ADDR, REG_ROUTING_MATRIX_OUTPUTS_IC_1_BYTE, (uint8_t *)R28_ROUTING_MATRIX_OUTPUTS_IC_1_Default );
  DSP_Write( REG_SERIAL_DATAGPIO_PIN_CONFIG_IC_1_ADDR, REG_SERIAL_DATAGPIO_PIN_CONFIG_IC_1_BYTE, (uint8_t *)R29_SERIAL_DATAGPIO_PIN_CONFIG_IC_1_Default );
  DSP_Write( REG_DSP_SLEW_MODES_IC_1_ADDR, REG_DSP_SLEW_MODES_IC_1_BYTE, (uint8_t *)R30_DSP_SLEW_MODES_IC_1_Default );
  DSP_Write( REG_SERIAL_PORT_SAMPLE_RATE_SETTING_IC_1_ADDR, REG_SERIAL_PORT_SAMPLE_RATE_SETTING_IC_1_BYTE, (uint8_t *)R31_SERIAL_PORT_SAMPLE_RATE_SETTING_IC_1_Default );
  DSP_Write( REG_CLOCK_ENABLE_REG_0_IC_1_ADDR , R32_CLOCK_ENABLE_REGISTERS_IC_1_SIZE, (uint8_t *)R32_CLOCK_ENABLE_REGISTERS_IC_1_Default );
  
  for (uint16_t i = 0; i < (PROGRAM_SIZE_IC_1 / ADAU_PROGRAM_DATA_SIZE); i++)
    DSP_Write( PROGRAM_ADDR_IC_1 + i, ADAU_PROGRAM_DATA_SIZE /*PROGRAM_SIZE_IC_1*/, (uint8_t *)(&Program_Data_IC_1[ADAU_PROGRAM_DATA_SIZE * i]));
  
  for (uint16_t i = 0; i < (PARAM_SIZE_IC_1 / ADAU_PARAM_DATA_SIZE); i++)
    DSP_Write( PARAM_ADDR_IC_1 + i, ADAU_PARAM_DATA_SIZE /*PARAM_SIZE_IC_1*/, (uint8_t *)(&Param_Data_IC_1[ADAU_PARAM_DATA_SIZE * i]) );
  
  DSP_Write( NON_MODULO_RAM_ADDR_IC_1, NON_MODULO_RAM_SIZE_IC_1, (uint8_t *)NON_MODULO_RAM_Data_IC_1 );
  
  uint8_t ReadData[6];
  I2C_Read(0x4002, 6, ReadData);
  
  DSP_Write( REG_SAMPLE_RATE_SETTING_IC_1_ADDR, REG_SAMPLE_RATE_SETTING_IC_1_BYTE, (uint8_t *)R36_SAMPLE_RATE_SETTING_IC_1_Default );
  DSP_Write( REG_DSP_RUN_REGISTER_IC_1_ADDR, REG_DSP_RUN_REGISTER_IC_1_BYTE, (uint8_t *)R37_DSP_RUN_REGISTER_IC_1_Default );
  DSP_Write( REG_DEJITTER_REGISTER_CONTROL_IC_1_ADDR, REG_DEJITTER_REGISTER_CONTROL_IC_1_BYTE, (uint8_t *)R38_DEJITTER_REGISTER_CONTROL_IC_1_Default );
  DSP_Write( REG_DEJITTER_REGISTER_CONTROL_IC_1_ADDR, REG_DEJITTER_REGISTER_CONTROL_IC_1_BYTE, (uint8_t *)R39_DEJITTER_REGISTER_CONTROL_IC_1_Default );
}
//------------------------------------------------------------------------------
void UpdateVolumeValue(uint16_t ADCValue)
{
  //ADCValue = ADC_MAX_VALUE - ADCValue;
  MainVolume = ADAU_PARAM_DATA_SIZE * (ADCValue / ADC_STEP_PER_VOLUME_UNIT);
}
//------------------------------------------------------------------------------
void DSP_UpdateVolume()
{
  uint8_t LeftVolume = MainVolume * BalanceLeft / BALANCE_MAX / ADAU_PARAM_DATA_SIZE;
  LeftVolume *= ADAU_PARAM_DATA_SIZE;
  
  uint8_t RightVolume = MainVolume * BalanceRight / BALANCE_MAX / ADAU_PARAM_DATA_SIZE;
  RightVolume *= ADAU_PARAM_DATA_SIZE;
  
  ADAU1761_SafeLoad(ADAU1761_LEFT_VOL_ADR, (uint8_t*)(&ADAU_VOLUME[LeftVolume]), ADAU1761_SAFELOAD_NUMLOAD_1);
  ADAU1761_SafeLoad(ADAU1761_LEFT_VOL_STEP_ADR, (uint8_t*)ADAU_VOLUME_STEP, ADAU1761_SAFELOAD_NUMLOAD_1);
  ADAU1761_SafeLoad(ADAU1761_RIGHT_VOL_ADR, (uint8_t*)(&ADAU_VOLUME[RightVolume]), ADAU1761_SAFELOAD_NUMLOAD_1);
  ADAU1761_SafeLoad(ADAU1761_RIGHT_VOL_STEP_ADR, (uint8_t*)ADAU_VOLUME_STEP, ADAU1761_SAFELOAD_NUMLOAD_1);
}
//------------------------------------------------------------------------------
void DSP_ChangeEQParam(uint8_t POTNum, uint16_t ADCValue)
{
  //ADCValue = ADC_MAX_VALUE - ADCValue;
  
  uint16_t ParamIndex = ADAU_PARAM_DATA_SIZE * ADAU_EQ_PARAMS_COUNT * (ADCValue / UserPOTs[POTNum].ADCStep);
  ADAU1761_SafeLoad(UserPOTs[POTNum].ParamAdr, &(UserPOTs[POTNum].ParamData[ParamIndex]), ADAU1761_SAFELOAD_NUMLOAD_5);
}
//------------------------------------------------------------------------------
void DSP_Mux(uint8_t Flag)
{
  if (Flag == EFFECT_ON)
  {
    ADAU1761_SafeLoad(ADAU1761_MUX_1_0_ADR, (uint8_t *)ADAU_VALUE_ZERO, ADAU1761_SAFELOAD_NUMLOAD_1);
    ADAU1761_SafeLoad(ADAU1761_MUX_1_1_ADR, (uint8_t *)ADAU_VALUE_ONE, ADAU1761_SAFELOAD_NUMLOAD_1);
  }
  else
  {
    ADAU1761_SafeLoad(ADAU1761_MUX_1_0_ADR, (uint8_t *)ADAU_VALUE_ONE, ADAU1761_SAFELOAD_NUMLOAD_1);
    ADAU1761_SafeLoad(ADAU1761_MUX_1_1_ADR, (uint8_t *)ADAU_VALUE_ZERO, ADAU1761_SAFELOAD_NUMLOAD_1);
  }
}
//------------------------------------------------------------------------------
void DSP_Mute(uint8_t Flag)
{
  uint8_t *ParamData;
  if (Flag == EFFECT_ON)
    ParamData = (uint8_t*)ADAU_VALUE_ZERO;
  else
    ParamData = (uint8_t*)ADAU_VALUE_ONE;
  
  ADAU1761_SafeLoad(ADAU1761_MUTE_ADR, ParamData, ADAU1761_SAFELOAD_NUMLOAD_1);
  DSP_Write( NON_MODULO_RAM_ADDR_IC_1, NON_MODULO_RAM_SIZE_IC_1, (uint8_t *)NON_MODULO_RAM_Data_IC_1 );
}
//------------------------------------------------------------------------------
void DSP_Bass(uint8_t Flag)
{
  uint8_t *ParamData;
  if (Flag == EFFECT_ON)
    ParamData = (uint8_t*)ADAU_VALUE_ZERO;
  else
    ParamData = (uint8_t*)ADAU_VALUE_ONE;
  
  DSP_Write(ADAU1761_BASS_ENABLE_ADR, 4, ParamData);
}
//------------------------------------------------------------------------------
void ADAU1761_SafeLoad(uint16_t ParamAddr, uint8_t* ParamData, uint8_t NumLoad)
{
  for (uint8_t i = 0; i < NumLoad; i++)
    DSP_Write(ADAU1761_SAFELOAD_DATA_START_ADR + i, ADAU_PARAM_DATA_SIZE, &ParamData[i * ADAU_PARAM_DATA_SIZE]);
  
  ParamAddr -= 1;
  
  uint8_t Data[4];
  Data[0] = 0;
  Data[1] = 0;
  Data[2] = ParamAddr >> 8;
  Data[3] = ParamAddr;
  DSP_Write(ADAU1761_SAFELOAD_PARAM_ADR, 4, Data);
  
  Data[0] = 0;
  Data[1] = 0;
  Data[2] = 0;
  Data[3] = NumLoad;
  DSP_Write(ADAU1761_SAFELOAD_NUMLOAD, 4, Data);
}


#elif defined(ADAU1701_DSP)

void DSP_Init()
{
  DSP_Write( REG_COREREGISTER_IC_1_ADDR, REG_COREREGISTER_IC_1_BYTE, (uint8_t *)R0_COREREGISTER_IC_1_Default );
  
  for (uint16_t i = 0; i < (PROGRAM_SIZE_IC_1 / ADAU_PROGRAM_DATA_SIZE); i++)
    DSP_Write( PROGRAM_ADDR_IC_1 + i, ADAU_PROGRAM_DATA_SIZE, (uint8_t *)(&Program_Data_IC_1[ADAU_PROGRAM_DATA_SIZE * i]));
  
  for (uint16_t i = 0; i < (PARAM_SIZE_IC_1 / ADAU_PARAM_DATA_SIZE); i++)
    DSP_Write( PARAM_ADDR_IC_1 + i, ADAU_PARAM_DATA_SIZE, (uint8_t *)(&Param_Data_IC_1[ADAU_PARAM_DATA_SIZE * i]) );
  
  DSP_Write( REG_COREREGISTER_IC_1_ADDR , R3_HWCONFIGURATION_IC_1_SIZE, (uint8_t *)R3_HWCONFIGURATION_IC_1_Default );
  DSP_Write( REG_COREREGISTER_IC_1_ADDR, REG_COREREGISTER_IC_1_BYTE, (uint8_t *)R4_COREREGISTER_IC_1_Default );
}
//------------------------------------------------------------------------------
void UpdateVolumeValue(uint16_t ADCValue)
{
  //ADCValue = ADC_MAX_VALUE - ADCValue;
  MainVolume = ADAU_PARAM_DATA_SIZE * (ADCValue / ADC_STEP_PER_VOLUME_UNIT);
}
//------------------------------------------------------------------------------
void DSP_UpdateVolume()
{
  uint8_t LeftVolume = MainVolume * BalanceLeft / BALANCE_MAX / ADAU_PARAM_DATA_SIZE;
  LeftVolume *= ADAU_PARAM_DATA_SIZE;
  
  uint8_t RightVolume = MainVolume * BalanceRight / BALANCE_MAX / ADAU_PARAM_DATA_SIZE;
  RightVolume *= ADAU_PARAM_DATA_SIZE;
  
  ADAU1701_SafeLoadInit(ADAU1701_SAFELOAD_ADR_0, ADAU1701_SAFELOAD_DATA_0, ADAU1701_LEFT_VOL_ADR, (uint8_t*)(&ADAU_VOLUME[LeftVolume]));
  ADAU1701_SafeLoadInit(ADAU1701_SAFELOAD_ADR_0 + 1, ADAU1701_SAFELOAD_DATA_0 + 1, ADAU1701_LEFT_VOL_STEP_ADR, (uint8_t*)ADAU_VOLUME_STEP);
  ADAU1701_SafeLoadInit(ADAU1701_SAFELOAD_ADR_0 + 2, ADAU1701_SAFELOAD_DATA_0 + 2, ADAU1701_RIGHT_VOL_ADR, (uint8_t*)(&ADAU_VOLUME[RightVolume]));
  ADAU1701_SafeLoadInit(ADAU1701_SAFELOAD_ADR_0 + 3, ADAU1701_SAFELOAD_DATA_0 + 3, ADAU1701_RIGHT_VOL_STEP_ADR, (uint8_t*)ADAU_VOLUME_STEP);
  ADAU1701_SafeLoadTransfer();
}
//------------------------------------------------------------------------------
void DSP_ChangeEQParam(uint8_t POTNum, uint16_t ADCValue)
{
  //ADCValue = ADC_MAX_VALUE - ADCValue;  
  uint16_t ParamIndex = ADAU_PARAM_DATA_SIZE * ADAU_EQ_PARAMS_COUNT * (ADCValue / UserPOTs[POTNum].ADCStep);
  
  for (uint8_t i = 0; i < ADAU_EQ_PARAMS_COUNT; i++)
    ADAU1701_SafeLoadInit(ADAU1701_SAFELOAD_ADR_0 + i, ADAU1701_SAFELOAD_DATA_0 + i, UserPOTs[POTNum].ParamAdr + i, &(UserPOTs[POTNum].ParamData[ParamIndex + i * ADAU_PARAM_DATA_SIZE]));
  
  ADAU1701_SafeLoadTransfer();
}
//------------------------------------------------------------------------------
void DSP_Mux(uint8_t Flag)
{
  if (Flag == EFFECT_ON)
  {
    DSP_Write(ADAU1701_MUX_1_0_ADR, ADAU_PARAM_DATA_SIZE, (uint8_t *)ADAU_VALUE_ONE);
    DSP_Write(ADAU1701_MUX_1_1_ADR, ADAU_PARAM_DATA_SIZE, (uint8_t *)ADAU_VALUE_ZERO);
  }
  else
  {
    DSP_Write(ADAU1701_MUX_1_0_ADR, ADAU_PARAM_DATA_SIZE, (uint8_t *)ADAU_VALUE_ZERO);
    DSP_Write(ADAU1701_MUX_1_1_ADR, ADAU_PARAM_DATA_SIZE, (uint8_t *)ADAU_VALUE_ONE);
  }
}
//------------------------------------------------------------------------------
void DSP_Mute(uint8_t Flag)
{
  uint8_t *ParamData;
  if (Flag == EFFECT_ON)
    ParamData = (uint8_t*)ADAU_VALUE_ZERO;
  else
    ParamData = (uint8_t*)ADAU_VALUE_ONE;
  
  DSP_Write(ADAU1701_MUTE_ADR, ADAU_PARAM_DATA_SIZE, ParamData);
}
//------------------------------------------------------------------------------
void DSP_Bass(uint8_t Flag)
{
  uint8_t *ParamData;
  if (Flag == EFFECT_ON)
    ParamData = (uint8_t*)ADAU_VALUE_ZERO;
  else
    ParamData = (uint8_t*)ADAU_VALUE_ONE;
  
  DSP_Write(ADAU1701_BASS_ENABLE_ADR, ADAU_PARAM_DATA_SIZE, ParamData);
}
//------------------------------------------------------------------------------
void ADAU1701_SafeLoadInit(uint16_t SafeAddrAddr, uint16_t SafeDataAddr, uint16_t ParamAddr, uint8_t* ParamData)
{
  uint8_t Data[5];
  Data[0] = 0;
          
  Data[1] = ParamAddr;
  DSP_Write(SafeAddrAddr, 2, Data);
  
  Data[1] = ParamData[0];
  Data[2] = ParamData[1];
  Data[3] = ParamData[2];
  Data[4] = ParamData[3];
  DSP_Write(SafeDataAddr, 5, Data);
}
//------------------------------------------------------------------------------
void ADAU1701_SafeLoadTransfer()
{
  uint8_t Data[2];
  Data[0] = 0;
  
  I2C_Read(ADAU1701_DSP_CORE_CTRL, 2, Data);
  Data[1] |= ADAU1701_DSP_CORE_IST;
  DSP_Write(ADAU1701_DSP_CORE_CTRL, 2, Data);
}


#endif

