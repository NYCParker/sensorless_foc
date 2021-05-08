#include <XMC4400.h>
#include <xmc_gpio.h>
#include "MCU_Initialize.h"
#include "Call_Functions.h"
#include "VAR_Initialize.h"
#include "Motor_Functions.h"
#include "PI_Control.h"
#include "SVPWM.h"
#include "DAC.h"
#include "Agreement.h"


/******************************************************************************************************************/
void CCU80_0_IRQHandler(void)
{	
	switch(Motor.flag.state)
		
	{       /**********************停止状态*********************/
			case Motor_Stop:                                 
			{
				CCU80_CC80->CR1S	=	SVPWM16.PWM_Period;
				CCU80_CC81->CR1S	=	SVPWM16.PWM_Period+1;
				CCU80_CC82->CR1S	=	SVPWM16.PWM_Period+1;      
				CCU80->GCSS |=0x00000111;	
				break;
			}
			
			/**********************定位状态*********************/
			case Motor_Lock:                                  
			{
				CCU80_CC80->CR1S	=	SVPWM16.PDC_U;
				CCU80_CC81->CR1S	=	SVPWM16.PDC_V;
				CCU80_CC82->CR1S	=	SVPWM16.PDC_W;
				CCU80->GCSS |=0x00001111;
				Thetae_Lock_CCU8();
				break;
			}
	
			/**********************I/F状态*********************/
			case Motor_Start:                                
			{
				CCU80_CC80->CR1S	=	SVPWM16.PDC_U;
				CCU80_CC81->CR1S	=	SVPWM16.PDC_V;
				CCU80_CC82->CR1S	=	SVPWM16.PDC_W;
				CCU80->GCSS |=0x00001111;				
                Thetae_Inc_CCU8();
				break;
			}
			
			/**********************Sensorless_FOC状态*********************/
			case Motor_Run:                                  
			{
				CCU80_CC80->CR1S	=	SVPWM16.PDC_U;
				CCU80_CC81->CR1S	=	SVPWM16.PDC_V;
				CCU80_CC82->CR1S	=	SVPWM16.PDC_W;
				CCU80->GCSS |=0x00001111;
				break;
			}				
	    default:
	    break;
	}
	SET_BIT(CCU80_CC80->SWR, CCU8_CC8_SWR_ROM_Pos);	
}
/*******************************************************************************************************************/



/*******************************************************************************************************************/
void VADC0_G0_0_IRQHandler(void)
{
	GlobalVar.R_Value = ((VADC_G0->RESD[1])&0x0000FFFF); //电位器采样 
	Re_Constuct_3_Phase_Current();                       //重构三相电流
	
    switch(Motor.flag.state)
	{
		/**********************定位状态*********************/
		case Motor_Lock:                                
		{                
			Iuvw_to_Idq(0);
			PI_Id_Loop();
			PI_Iq_Loop();
			Udq_to_UarphaUbeta(0);
			SVPWM16_7(Motor.info.i16_Ua,Motor.info.i16_Ub); 
			break;
		}
		
		/**********************I/F状态*********************/
		case Motor_Start:                              
		{
			
			ASMO();                                //滑模角度观测器
			SMO_Speed_Est();                       //估计转速
			SMOIF_Thetae_Delay();                  //得到滑模和IF的角度差       
			ASpeed_Filter();	                   //角速度滤波
			SMO_IF_Filter();                       //滑模和IF的角度差滤波
			
			Iuvw_to_Idq(Motor.info.i16_Thetae_IF);
			PI_Id_Loop();
			PI_Iq_Loop();
		    Udq_to_UarphaUbeta(Motor.info.i16_Thetae_IF);
		    SVPWM16_7(Motor.info.i16_Ua,Motor.info.i16_Ub);

 			Data_Record();                         //虚拟示波器 
			break;
		}
		
		/**********************Sensorless_FOC状态*********************/
		case Motor_Run:                                  
		{	
			ASMO();             				
			SMO_Speed_Est();     				
            ASpeed_Filter();	     				
			
			if(SMO.Switch == 0)                    //切换过程处理，只做一次
			{
				IF_SMO_Switch();                   //双dq变换保证切换前后矢量不变
			}

			Iuvw_to_Idq(SMO.i16_Thetae_Con);
			PI_Id_Loop();
			PI_Iq_Loop();
			Udq_to_UarphaUbeta(SMO.i16_Thetae_Con);
			SVPWM16_7(Motor.info.i16_Ua,Motor.info.i16_Ub);
			
			SMO.speedloop_count++;
			if(SMO.speedloop_count>=16)
			{
		        SMO.speedloop_count = 0;
				
				if(Control_Loop_Mode == 1)
				{
					Motor.cmd.i16_Iq = Current_Cmd_Iq;
				}
				else if(Control_Loop_Mode == 2)
				{
					PI_Speed_Loop();
				}
				
				Motor.cmd.i16_Id = (Motor.cmd.i16_Id*62)>>8;
			}
			
			Data_Record();
			DAC_Output1(SMO.i16_Thetae_Con>>1); 
			break;
		}		
		default:
		break;
	}	

	SET_BIT(VADC_G0->REFCLR, VADC_G_REFLAG_REV0_Pos);
	SET_BIT(VADC_G0->REFCLR, VADC_G_REFLAG_REV3_Pos);
	SET_BIT(VADC_G0->REFCLR, VADC_G_REFLAG_REV1_Pos);	
	SET_BIT(VADC_G2->REFCLR, VADC_G_REFLAG_REV3_Pos);
	SET_BIT(VADC_G3->REFCLR, VADC_G_REFLAG_REV3_Pos); 
}
/*******************************************************************************************************************/



/*******************************************************************************************************************/
void SysTick_Handler(void)
{
	if(hand_shake_Flag)
	{
	    if(GlobalVar.Record_Flag != 2)
		{
			Communication_App();      //串口向电脑发送数据
		}
		else
		{
			Virtual_Scope();          //虚拟示波器
		}
	}	
}
/*******************************************************************************************************************/

