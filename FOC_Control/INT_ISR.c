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
		
	{       /**********************ֹͣ״̬*********************/
			case Motor_Stop:                                 
			{
				CCU80_CC80->CR1S	=	SVPWM16.PWM_Period;
				CCU80_CC81->CR1S	=	SVPWM16.PWM_Period+1;
				CCU80_CC82->CR1S	=	SVPWM16.PWM_Period+1;      
				CCU80->GCSS |=0x00000111;	
				break;
			}
			
			/**********************��λ״̬*********************/
			case Motor_Lock:                                  
			{
				CCU80_CC80->CR1S	=	SVPWM16.PDC_U;
				CCU80_CC81->CR1S	=	SVPWM16.PDC_V;
				CCU80_CC82->CR1S	=	SVPWM16.PDC_W;
				CCU80->GCSS |=0x00001111;
				Thetae_Lock_CCU8();
				break;
			}
	
			/**********************I/F״̬*********************/
			case Motor_Start:                                
			{
				CCU80_CC80->CR1S	=	SVPWM16.PDC_U;
				CCU80_CC81->CR1S	=	SVPWM16.PDC_V;
				CCU80_CC82->CR1S	=	SVPWM16.PDC_W;
				CCU80->GCSS |=0x00001111;				
                Thetae_Inc_CCU8();
				break;
			}
			
			/**********************Sensorless_FOC״̬*********************/
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
	GlobalVar.R_Value = ((VADC_G0->RESD[1])&0x0000FFFF); //��λ������ 
	Re_Constuct_3_Phase_Current();                       //�ع��������
	
    switch(Motor.flag.state)
	{
		/**********************��λ״̬*********************/
		case Motor_Lock:                                
		{                
			Iuvw_to_Idq(0);
			PI_Id_Loop();
			PI_Iq_Loop();
			Udq_to_UarphaUbeta(0);
			SVPWM16_7(Motor.info.i16_Ua,Motor.info.i16_Ub); 
			break;
		}
		
		/**********************I/F״̬*********************/
		case Motor_Start:                              
		{
			
			ASMO();                                //��ģ�Ƕȹ۲���
			SMO_Speed_Est();                       //����ת��
			SMOIF_Thetae_Delay();                  //�õ���ģ��IF�ĽǶȲ�       
			ASpeed_Filter();	                   //���ٶ��˲�
			SMO_IF_Filter();                       //��ģ��IF�ĽǶȲ��˲�
			
			Iuvw_to_Idq(Motor.info.i16_Thetae_IF);
			PI_Id_Loop();
			PI_Iq_Loop();
		    Udq_to_UarphaUbeta(Motor.info.i16_Thetae_IF);
		    SVPWM16_7(Motor.info.i16_Ua,Motor.info.i16_Ub);

 			Data_Record();                         //����ʾ���� 
			break;
		}
		
		/**********************Sensorless_FOC״̬*********************/
		case Motor_Run:                                  
		{	
			ASMO();             				
			SMO_Speed_Est();     				
            ASpeed_Filter();	     				
			
			if(SMO.Switch == 0)                    //�л����̴���ֻ��һ��
			{
				IF_SMO_Switch();                   //˫dq�任��֤�л�ǰ��ʸ������
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
			Communication_App();      //��������Է�������
		}
		else
		{
			Virtual_Scope();          //����ʾ����
		}
	}	
}
/*******************************************************************************************************************/

