
/*
main函数
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
	
	/****系统定时器时基中断****/
	SysTick_Config(SystemCoreClock/1000);
	
	
	/****外设驱动初始化与测试****/
	GPIO_Init();
	ADC_Init();
	CCU80_Init();
	POSIF0_Init();
	DAC_Init();
	UART_Init();	
	DAC_Output1(2000);                 
	delay_Xms(200);	                  
	
    /****运行前校准****/
	Cur_Calibration();
	NVIC_Handle();
	__enable_irq();
	
	/****启用定时器****/
	CCU80_Start();
	
	while(1)
	{
		Serial_Communication();
		Motor_State_Commmand();

		switch(Motor.flag.state)
		{
			/**********************停止状态*********************/
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
					Motor.cmd.i16_Iq = 420;          //定位   
				}
				break;	
			}
			
			/**********************定位状态*********************/
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
					Motor.cmd.i16_Iq = 250;           //启动
				}               				
                break;				
			}
			
			/**********************I/F状态*********************/
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
					Motor.flag.state = Motor_Run;     //运行
				}               				
                break;				
			}
			
			/**********************Sensorless_FOC状态*********************/
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

