
/*
DAC 配置
14.8  DAC0
14.9  DAC1
14.8口与w相电流采样冲突 只使用14.9进行DA输出
*/

#include "DAC.h"
#include <xmc_dac.h>

XMC_DAC_CH_CONFIG_t const ch_config0=
{
  .output_offset	= 0U,
  .data_type 		= XMC_DAC_CH_DATA_TYPE_SIGNED,
  .output_scale 	= XMC_DAC_CH_OUTPUT_SCALE_NONE,
  .output_negation  = XMC_DAC_CH_OUTPUT_NEGATION_DISABLED,
};

XMC_DAC_CH_CONFIG_t const ch_config1=
{
  .output_offset	= 0U,
  .data_type 		= XMC_DAC_CH_DATA_TYPE_SIGNED,
  .output_scale 	= XMC_DAC_CH_OUTPUT_SCALE_NONE,
  .output_negation  = XMC_DAC_CH_OUTPUT_NEGATION_DISABLED,
};

void DAC_Init(void)
{
 //XMC_DAC_CH_Init(XMC_DAC0, 0U, &ch_config0);
 //XMC_DAC_CH_StartSingleValueMode(XMC_DAC0, 0U);
  XMC_DAC_CH_Init(XMC_DAC0, 1U, &ch_config1);
  XMC_DAC_CH_StartSingleValueMode(XMC_DAC0, 1U);	
}

void DAC_Output0(int16_t dat)
{
  XMC_DAC_CH_Write(XMC_DAC0,0U,dat);
}

void DAC_Output1(int16_t dat)
{
  XMC_DAC_CH_Write(XMC_DAC0,1U,dat);
}
