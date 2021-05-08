#include <XMC4400.h>
#include "Call_Functions.h"
#include "VAR_Initialize.h"
#include "Agreement.h"

uint16_t d_count;
int16_t d_time[d_Fil_Length];
int64_t d_sum;
int8_t  d_get_flag;    //用于平均滤波

uint16_t d1_count;
int16_t d1_time[d1_Fil_Length];
int64_t d1_sum;
int8_t  d1_get_flag;   //用于平均滤波

uint16_t SMO_Spdcnt;
int16_t SMO_Thetaeinc_Arr[SMO_Speed_Length];
int64_t SMO_Spd_Sum;   //用于估计转速
int16_t test11;
uint8_t  SMO_Spdflag; 

//Delay函数
void delay_Xms(uint16_t ticx)        //误差 0us
{
	uint16_t tici;
    unsigned char tica,ticb,ticc;
	
	for(tici=0; tici<ticx; tici++)
    {
	   for(ticc=1;ticc>0;ticc--)
         for(ticb=98;ticb>0;ticb--)
           for(tica=230;tica>0;tica--);
	}
}

//电机控制指令函数
void Motor_State_Commmand(void)      
{
	if(uart_state_flag == 0)
	{
		Motor.cmd.State = 0;
	}	
	
	if(Motor.cmd.State == 0&&uart_state_flag == 1)
	{
		Motor.cmd.State = 1;
	}
	
	if(Motor.cmd.State == 1&&Motor.flag.Lock_done == 1)
	{
		Motor.cmd.State = 2;
	}
	
	if(Motor.cmd.State == 2&&Motor.flag.Switch == 1)
	{
		Motor.cmd.State = 3;
	}
	
    //虚拟示波器
	if(GlobalVar.Record_Flag == 0 && GlobalVar.Start_Record == 1)
	{
		GlobalVar.Record_Flag = 1;
	}
	if(GlobalVar.Record_Flag == 1 && GlobalVar.Send_Record == 1)
	{
		GlobalVar.Record_Flag = 2;
	}
}

//平均滤波函数，这里用来对Luenberger的估计角速度进行滤波，可选择用于速度闭环
void ASpeed_Filter(void)
{
		if(d_count>(d_Fil_Length-1))
		{
			d_count=0;
			d_get_flag=1;
		}		
		if(d_get_flag==0)
		{
		  d_time[d_count]=SMO.i32_Speed_ASMO;
		  d_sum += d_time[d_count];
		}
		else
		{
			d_sum -=d_time[d_count];
			d_time[d_count] = SMO.i32_Speed_ASMO;
			d_sum += d_time[d_count];
			SMO.i32_Speed_ASMO_Fil = d_sum/d_Fil_Length;	
		}	
		d_count++;	
}

//平均滤波函数初始化
void ASpeed_Filter_Initialize(void)    
{
	uint16_t reset_d;
	SMO.i32_Speed_ASMO_Fil = 0;
	SMO.i32_Speed_ASMO = 0;
	
	for(reset_d=0;reset_d<d_Fil_Length;reset_d++)
	{
		d_time[reset_d]=0;
    }
	d_sum = 0;
	d_count=0;
	d_get_flag=0;	
}

//平均滤波函数，这里用来对I/F运行时的延迟角进行滤波，用于切换
void SMO_IF_Filter(void)
{
		if(d1_count>(d1_Fil_Length-1))
		{
			d1_count=0;
			d1_get_flag=1;
		}		
		if(d1_get_flag==0)
		{
		  d1_time[d1_count]=SMO.i16_Thetae_SMOIF_Delay;;
		  d1_sum += d1_time[d1_count];
		}
		else
		{
			d1_sum -=d1_time[d1_count];
			d1_time[d1_count] = SMO.i16_Thetae_SMOIF_Delay;
			d1_sum += d1_time[d1_count];
			SMO.i16_Thetae_SMOIF_Delay_Fil = d1_sum/d1_Fil_Length;	
		}	
		d1_count++;	
}

void SMO_IF_Filter_Initialize(void)    
{
	uint16_t reset_d1;
	
	SMO.i16_Thetae_SMOIF_Delay = 0;
	SMO.i16_Thetae_SMOIF_Delay_Fil = 0;
	
	for(reset_d1=0;reset_d1<d1_Fil_Length;reset_d1++)
	{
		d1_time[reset_d1]=0;
    }
	d1_sum = 0;
	d1_count=0;
	d1_get_flag=0;	
}

//滑模转速估计
void SMO_Speed_Est(void)
{
	if(SMO_Spdcnt > (SMO_Speed_Length-1))
	{
		SMO_Spdcnt = 0;
		SMO_Spdflag = 1;
	}
	
	if(SMO_Spdflag == 0)
	{
		if(-2000<(SMO.i16_Thetae - SMO.i16_Thetae_Last)&&(SMO.i16_Thetae - SMO.i16_Thetae_Last)<0)
		{
		SMO_Thetaeinc_Arr[SMO_Spdcnt] = 5;
		}
		else
		{
		SMO_Thetaeinc_Arr[SMO_Spdcnt] = (SMO.i16_Thetae - SMO.i16_Thetae_Last+7200)%3600;
		}
		SMO_Spd_Sum +=  SMO_Thetaeinc_Arr[SMO_Spdcnt];
	}
	
	else
	{
		SMO_Spd_Sum  -= SMO_Thetaeinc_Arr[SMO_Spdcnt];
		test11 = (SMO.i16_Thetae - SMO.i16_Thetae_Last+7200)%3600;
		SMO_Thetaeinc_Arr[SMO_Spdcnt] = test11;
		SMO_Spd_Sum  += SMO_Thetaeinc_Arr[SMO_Spdcnt];
		SMO.i16_Speed_Est = (60000000*SMO_Spd_Sum)/(SMO_Speed_Length*62.5*2*360*10);
	} 
	SMO_Spdcnt++;
}

void SMO_Speed_Est_Init(void)    
{
	uint16_t reset_speed;
	
	SMO.i16_Speed_Est = 0;
	
	for(reset_speed=0;reset_speed<SMO_Speed_Length;reset_speed++)
	{
		SMO_Thetaeinc_Arr[reset_speed]=0;
    }
	SMO_Spd_Sum = 0;
	SMO_Spdcnt=0;
	SMO_Spdflag=0;	
}

uint8_t speed_count;
uint32_t speed_time[32];
uint64_t speed_time_sum;
uint8_t  speed_get_flag; 
uint16_t Interval;     //用于计算转速

//有位置传感器的情况下进行转速计算（无感不使用） 
void Speed_Calculate(void)
{
	if(speed_count>31)
		{
			speed_count=0;
			speed_get_flag=1;
		}		
		if(speed_get_flag==0)
		{
		  speed_time[speed_count]=(CCU40_CC43->CV[0]&0x0000FFFF);
		  speed_time_sum += speed_time[speed_count];
		}
		else
		{
			speed_time_sum -=speed_time[speed_count];
			speed_time[speed_count] = (CCU40_CC43->CV[0]&0x0000FFFF);
			speed_time_sum += speed_time[speed_count];
			Motor.info.ui16_Speed=(2250000*32)/(speed_time_sum);
		}	
		speed_count++;	
}
