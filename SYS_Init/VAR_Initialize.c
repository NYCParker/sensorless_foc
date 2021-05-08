/*
变量初始化配置
*/
#include <XMC4400.h>
#include "VAR_Initialize.h"
#include "Call_Functions.h"
#include "MCU_Initialize.h"
#include "SVPWM.h"

/****结构体定义****/
MOTORs Motor;
GlobalVars GlobalVar;
COUNTs Count;
SMOs SMO;

//PI参数与限幅初始值 //根据实际项目调参
PIDs PI_SMO =   {0,0,0.0022,0.0140,0,5000};
PIDs PI_Iq =    {0,0,0.12*2,0.0153*2,0,32000};  
PIDs PI_Id =    {0,0,0.12*2,0.0153*2,0,32000};
PIDs PI_Speed=  {0,0,0.1,0.0006,0,2500};      

void Initialize_Variables(void)
{
	SVPWM16.PWM_Period=3750;
	
	/****滤波与速度估计初始化****/
	ASpeed_Filter_Initialize();
	SMO_IF_Filter_Initialize();
	SMO_Speed_Est_Init();         
	
	/****电机指令初始化****/
	Motor.cmd.i16_Id = 0;
	Motor.cmd.i16_Iq = 0;
	Motor.cmd.i16_Speed = 3000;
	
	/****电机信息初始化****/
	Motor.info.i16_Id = 0;
    Motor.info.i16_Iq = 0;
	Motor.info.i16_Ud = 0;
	Motor.info.i16_Uq = 0;
	Motor.info.i16_Ua = 0;
	Motor.info.i16_Ub = 0;	
		
    /****电机状态位初始化****/
	Motor.flag.Lock_done = 0;
	Motor.flag.Switch = 0;
	
	/****F启动初始化****/
	GlobalVar.Lock_Time = 4000;
	Count.ui16_Thetae_IFlock = 0;
	Count.ui16_Thetae_IFInc_Interval = 0;
	Count.ui16_Thetae_Interval = 0;	
	Motor.info.i16_Thetae_IF = 0;
	Motor.info.i16_Thetae_Inc = 15;
	GlobalVar.Thetae_IFInc_Interval = 30000;
    GlobalVar.Thetae_Inc_Max = 15;
	GlobalVar.Thetae_InTerval = 5;
	
	/****虚拟示波器初始化****/
	GlobalVar.Data_Line = 0;
	GlobalVar.Data_Row = 0;
	GlobalVar.Record_Cnt = 0;
	GlobalVar.Record_Flag = 0;
	GlobalVar.Start_Record = 0;
	GlobalVar.Send_Record = 0;
	
	/****滑模观测器初始化****/
	SMO.i32_Ealpha = 0;
	SMO.i32_Ebeta = 0;
	SMO.i32_EstIalpha = 0;
	SMO.i32_EstIbeta = 0;
	SMO.i32_Ialpha = 0;
	SMO.i32_Ialpha_Error = 0;
	SMO.i32_Ibeta = 0;
	SMO.i32_Ibeta_Error = 0;
	SMO.i32_Ualpha = 0;
	SMO.i32_Ubeta = 0;                                                                 
	SMO.i32_Zalpha = 0;
	SMO.i32_Zbeta = 0;	
	
	/****滑模观测器参数初始化****/ //根据实际项目调参
	SMO.ui16_F = 491;  
	SMO.ui16_G = 52;
	SMO.ui16_kslide = 4;        
	SMO.i32_Zmax =30000;             
	SMO.i32_Emax =30000;	
	SMO.i16_k = 3000;
	SMO.Comp = 0;
	
	SMO.i16_Thetae = 0;
	SMO.i16_Thetae_Last = 0;
	SMO.i32_PLL_Error = 0;
	SMO.Switch = 0;
	SMO.speedloop_count = 0;
}

void Initialize_PI(PIDs *pid)
{
	pid->i32_e1 = 0;
	pid->i32_e2 = 0;
	pid->i32_output = 0;
}

void Initialize_All_Var(void)     //初始化所有参数
{
	Initialize_Variables();
	Initialize_PI(&PI_Id);
	Initialize_PI(&PI_Iq);
	Initialize_PI(&PI_SMO);
	Initialize_PI(&PI_Speed);
}

void Cur_Calibration(void)        //校正零偏电流ADC值
{
	int Cur_Calibration_i;
	int Cur_Calibration_Count;
	int Cur_Calibration_Iu_sum;
	int Cur_Calibration_Iv_sum;
	int Cur_Calibration_Iw_sum;
	
	Cur_Calibration_Count = 200;
	
	Cur_Calibration_Iu_sum = 0;
	Cur_Calibration_Iv_sum = 0;
	Cur_Calibration_Iw_sum = 0;	
	
	for(Cur_Calibration_i=0;Cur_Calibration_i<Cur_Calibration_Count;Cur_Calibration_i++)
	{
		WR_REG(VADC_G0->QMR0, VADC_G_QMR0_TREV_Msk, VADC_G_QMR0_TREV_Pos, 1);
		delay_Xms(2);
		while((RD_REG(VADC_G2->REFLAG, VADC_G_REFLAG_REV2_Msk, VADC_G_REFLAG_REV2_Pos) == 0)&&
			  (RD_REG(VADC_G3->REFLAG, VADC_G_REFLAG_REV2_Msk, VADC_G_REFLAG_REV2_Pos) == 0)&&
               RD_REG(VADC_G0->REFLAG, VADC_G_REFLAG_REV0_Msk, VADC_G_REFLAG_REV2_Pos) == 0){};
				   
		Cur_Calibration_Iu_sum = Cur_Calibration_Iu_sum + ((VADC_G0->RESD[2])&0x0000FFFF);		  
		Cur_Calibration_Iv_sum = Cur_Calibration_Iv_sum + ((VADC_G3->RESD[2])&0x0000FFFF);
		Cur_Calibration_Iw_sum = Cur_Calibration_Iw_sum + ((VADC_G2->RESD[2])&0x0000FFFF);
				   
		SET_BIT(VADC_G0->REFCLR, VADC_G_REFLAG_REV1_Pos);		  
		SET_BIT(VADC_G0->REFCLR, VADC_G_REFLAG_REV2_Pos);
		SET_BIT(VADC_G2->REFCLR, VADC_G_REFLAG_REV2_Pos);
		SET_BIT(VADC_G3->REFCLR, VADC_G_REFLAG_REV2_Pos); 	  
	}
	Motor.cst.ui16_Iu_ADCRef = Cur_Calibration_Iu_sum/(Cur_Calibration_Count);
	Motor.cst.ui16_Iv_ADCRef = Cur_Calibration_Iv_sum/(Cur_Calibration_Count);
	Motor.cst.ui16_Iw_ADCRef = Cur_Calibration_Iw_sum/(Cur_Calibration_Count);	
}
