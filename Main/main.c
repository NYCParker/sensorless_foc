
/*
main����
fSYS = 120Mhz
*/

#include <XMC4400.h>
#include "MCU_Initialize.h"
#include "VAR_Initialize.h"
#include "Call_Functions.h"
#include "Agreement.h"
#include "DAC.h"
#include "UART.h"
                                
int main(void)
{             
	__disable_irq();
	
	/****ϵͳ��ʱ��ʱ���ж�****/
	SysTick_Config(SystemCoreClock/1000);
	
	
	/****����������ʼ�������****/
	GPIO_Init();
	ADC_Init();
	CCU80_Init();
	POSIF0_Init();
	DAC_Init();
	UART_Init();	
	DAC_Output1(2000);                 
	delay_Xms(200);	                  
	
    /****����ǰУ׼****/
	Cur_Calibration();
	NVIC_Handle();
	__enable_irq();
	
	/****���ö�ʱ��****/
	CCU80_Start();
	
	while(1)
	{
		Serial_Communication();
		Motor_State_Commmand();

		switch(Motor.flag.state)
		{
			/**********************ֹͣ״̬*********************/
			case Motor_Stop:                        
			{
				if(Motor.cmd.State == 0)
				{
					Switch_off();
					Motor.flag.state = Motor_Stop;
					Initialize_All_Var();
				}
				
				if(Motor.cmd.State == 1)
				{
					Switch_on();
					Motor.flag.state = Motor_Lock;
					Motor.cmd.i16_Id = 0;
					Motor.cmd.i16_Iq = 420;          //��λ   
				}
				break;	
			}
			
			/**********************��λ״̬*********************/
			case Motor_Lock:                        
			{
				if(Motor.cmd.State == 0)
				{
					Switch_off();
					Motor.flag.state = Motor_Stop;
					Initialize_All_Var();
				}	
 				if(Motor.cmd.State == 2)
				{
					Motor.flag.state = Motor_Start;
					Motor.cmd.i16_Id = 0;
					Motor.cmd.i16_Iq = 250;           //����
				}               				
                break;				
			}
			
			/**********************I/F״̬*********************/
			case Motor_Start:                         
			{
				if(Motor.cmd.State == 0)
				{
					Switch_off();
					Motor.flag.state = Motor_Stop;
					Initialize_All_Var();
				}	
 				if(Motor.cmd.State == 3)
				{
					Motor.flag.state = Motor_Run;     //����
				}               				
                break;				
			}
			
			/**********************Sensorless_FOC״̬*********************/
			case Motor_Run:                          
			{
				if(Motor.cmd.State == 0)
				{
					Switch_off();
					Motor.flag.state = Motor_Stop;
					Initialize_All_Var();
				}				
                break;				
			}			
			default:break;
		}    		
	}
}

