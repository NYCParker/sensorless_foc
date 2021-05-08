#ifndef _Motor_Functions_h
#define _Motor_Functions_h

#include <XMC4400.h>
#include "VAR_Initialize.h"
#include "Call_Functions.h"
#include "PI_Control.h"
#include "MCU_Initialize.h"

extern const int16_t sintable16[3600];

static __INLINE int16_t COS(int16_t angle)
{
	angle += 900;
	if(angle >= 3600)
	    angle -= 3600;
    return sintable16[angle];
}

static __INLINE int16_t SIN(int16_t angle)
{
   if(angle >= 3600)
	angle -= 3600;
	return sintable16[angle];
}

static __INLINE void Re_Constuct_3_Phase_Current(void)
{
	Motor.info.ui16_Iu_ADC = ((VADC_G0->RESD[2])&0x0000FFFF);
	Motor.info.ui16_Iv_ADC = ((VADC_G3->RESD[2])&0x0000FFFF);
	Motor.info.ui16_Iw_ADC = ((VADC_G2->RESD[2])&0x0000FFFF);
	
	//直接采三相电流(未考虑SV窗口）
	Motor.info.i16_Iu = -(Motor.info.ui16_Iu_ADC - Motor.cst.ui16_Iu_ADCRef)*21/10;  //Iu     in Q15 format now  Ia 260 = 1A    Iq= 390=1A
    Motor.info.i16_Iv = -(Motor.info.ui16_Iv_ADC - Motor.cst.ui16_Iv_ADCRef)*21/10;  //Iv 
    Motor.info.i16_Iw = -(Motor.info.ui16_Iw_ADC - Motor.cst.ui16_Iw_ADCRef)*21/10;  //Iw
	
	if(Motor.info.i16_Iu>Current_Max ||  Motor.info.i16_Iv>Current_Max || Motor.info.i16_Iw>Current_Max || 
	   Motor.info.i16_Iu<-Current_Max||  Motor.info.i16_Iv<-Current_Max|| Motor.info.i16_Iw<-Current_Max )
	{
		Switch_off();
		Motor.flag.state = Motor_Stop;
	    Initialize_All_Var();
	}
}

static __INLINE void Iuvw_to_Idq(uint16_t anglee)
{
  uint16_t anglee120;
  uint16_t anglee240;
  int32_t i32_Id;
  int32_t i32_Iq;

  anglee120 = (anglee + 1200)%3600;
  anglee240 = (anglee + 2400)%3600;
  
  i32_Id = (((Motor.info.i16_Iu*COS(anglee))>>1) + (Motor.info.i16_Iv*COS(anglee240)>>1) + (Motor.info.i16_Iw*COS(anglee120)>>1));	
  i32_Iq =-(  ((Motor.info.i16_Iu*SIN(anglee))>>1)  + ((Motor.info.i16_Iv*SIN(anglee240))>>1)  +((Motor.info.i16_Iw*SIN(anglee120))>>1)  ); 
	
  Motor.info.i16_Id=i32_Id>>14;
  Motor.info.i16_Iq=i32_Iq>>14;    //260*3/2  =  1A = 390
}

static __INLINE void Udq_to_UarphaUbeta(uint16_t anglee)
{
	int32_t i32_Ua;
	int32_t i32_Ub;
	
	i32_Ua = ((Motor.info.i16_Ud*COS(anglee))>>1) - ((Motor.info.i16_Uq*SIN(anglee))>>1);
    i32_Ub = ((Motor.info.i16_Ud*SIN(anglee))>>1) + ((Motor.info.i16_Uq*COS(anglee))>>1);
	
	Motor.info.i16_Ua = i32_Ua>>14;
	Motor.info.i16_Ub = i32_Ub>>14;
}

static __INLINE void Thetae_Lock_CCU8(void)   //如果IF启动时的角度定位放在CCU8里做 2s定位时间
{
	Count.ui16_Thetae_IFlock++;
	
	if(Count.ui16_Thetae_IFlock == GlobalVar.Lock_Time)
	{
		Count.ui16_Thetae_IFlock = 0;
		
		Motor.flag.Lock_done = 1;
	}
}

static __INLINE void Thetae_Inc_CCU8(void)    //如果IF启动时的角度累计放在CCU8里做
{
	//Phie_Inc每隔0.625s*x + 1  
	Count.ui16_Thetae_IFInc_Interval++;
	
	if(Count.ui16_Thetae_IFInc_Interval == GlobalVar.Thetae_IFInc_Interval)
	{
		Motor.info.i16_Thetae_Inc++;
		Count.ui16_Thetae_IFInc_Interval = 0;
	}
	
	if(Motor.info.i16_Thetae_Inc >= GlobalVar.Thetae_Inc_Max)
	{
		Motor.info.i16_Thetae_Inc = GlobalVar.Thetae_Inc_Max;
	}

	//Phie 每隔  62.5*y us + Phie_Inc
	Count.ui16_Thetae_Interval++;
	
	if(Count.ui16_Thetae_Interval == GlobalVar.Thetae_InTerval)
	{
		Motor.info.i16_Thetae_IF = (Motor.info.i16_Thetae_IF+Motor.info.i16_Thetae_Inc+3600)%3600;
		Count.ui16_Thetae_Interval = 0;
	}	
	
	Count.ui16_Thetae_IFSwitch++;
	
	if(Count.ui16_Thetae_IFSwitch == 4000)
	{
		Count.ui16_Thetae_IFSwitch = 0;
		
	Motor.flag.Switch = 1;
	}	
}

static __INLINE void IF_Thetae_Delay(void)    
{
	int last;
	
	last = Motor.info.i16_Thetae_IF_Delay;
	
	Motor.info.i16_Thetae_IF_Delay = (Motor.info.i16_Thetae - Motor.info.i16_Thetae_IF+3600)%3600;
	if(Motor.info.i16_Thetae_IF_Delay>2000)
	{
		Motor.info.i16_Thetae_IF_Delay = Motor.info.i16_Thetae_IF_Delay - 3600;
	}
	Motor.info.i16_Thetae_IF_Delay_Fil = ((Motor.info.i16_Thetae_IF_Delay*80) + (last*20))/100;
}

static __INLINE void ASMO(void)    //滑模角度观测器   (k+1)
{
	SMO.i32_Ialpha = (Motor.info.i16_Iu*16384-(Motor.info.i16_Iv*8192)-(Motor.info.i16_Iw*8192))>>13; 
	SMO.i32_Ibeta = ((Motor.info.i16_Iv-Motor.info.i16_Iw)*14189)>>13;
    SMO.i32_Ualpha = Motor.info.i16_Ua;
	SMO.i32_Ubeta = Motor.info.i16_Ub;  

	SMO.i32_EstIalpha = (SMO.ui16_F*SMO.i32_EstIalpha + SMO.ui16_G*(SMO.i32_Ualpha - SMO.i32_Ealpha - SMO.i32_Zalpha))>>9;
    SMO.i32_EstIbeta = (SMO.ui16_F*SMO.i32_EstIbeta + SMO.ui16_G*(SMO.i32_Ubeta - SMO.i32_Ebeta - SMO.i32_Zbeta))>>9;	
	
	SMO.i32_Ialpha_Error=SMO.i32_EstIalpha-SMO.i32_Ialpha;
	SMO.i32_Ibeta_Error=SMO.i32_EstIbeta-SMO.i32_Ibeta;	

	SMO.i32_Zalpha = (SMO.i32_Ialpha_Error*SMO.ui16_kslide)>>4;
	SMO.i32_Zbeta  = (SMO.i32_Ibeta_Error*SMO.ui16_kslide)>>4;         //Z信号
	
	if(SMO.i32_Zalpha>SMO.i32_Zmax)	SMO.i32_Zalpha=SMO.i32_Zmax;
	else if(SMO.i32_Zalpha<-SMO.i32_Zmax) SMO.i32_Zalpha=-SMO.i32_Zmax;

	if(SMO.i32_Zbeta>SMO.i32_Zmax) SMO.i32_Zbeta=SMO.i32_Zmax;
	else if(SMO.i32_Zbeta<-SMO.i32_Zmax) SMO.i32_Zbeta=-SMO.i32_Zmax;

	SMO.i32_Ealpha =SMO.i32_Ealpha + (-SMO.i32_Speed_ASMO*SMO.i32_Ebeta - SMO.i16_k*(SMO.i32_Ealpha-SMO.i32_Zalpha))/16000;
	SMO.i32_Ebeta =SMO.i32_Ebeta +  (SMO.i32_Speed_ASMO*SMO.i32_Ealpha - SMO.i16_k*(SMO.i32_Ebeta-SMO.i32_Zbeta))/16000;	
	SMO.i32_Speed_ASMO  = SMO.i32_Speed_ASMO + ((SMO.i32_Ealpha-SMO.i32_Zalpha)*SMO.i32_Ebeta - (SMO.i32_Ebeta-SMO.i32_Zbeta)*SMO.i32_Ealpha)/16000;
	
	if(SMO.i32_Ealpha>SMO.i32_Emax)	SMO.i32_Ealpha=SMO.i32_Emax;
	else if(SMO.i32_Ealpha<-SMO.i32_Emax) SMO.i32_Ealpha=-SMO.i32_Emax;

	if(SMO.i32_Ebeta>SMO.i32_Emax) SMO.i32_Ebeta=SMO.i32_Emax;
	else if(SMO.i32_Ebeta<-SMO.i32_Emax) SMO.i32_Ebeta=-SMO.i32_Emax;	
	
	SMO.i32_PLL_Error = (-SMO.i32_Ealpha*COS(SMO.i16_Thetae) - SMO.i32_Ebeta*SIN(SMO.i16_Thetae))>>12;
	
	SMO.i16_Thetae_Last = SMO.i16_Thetae;  //记录上次角度
	
	PI_SMO_PLL(); 
	
	SMO.i16_Thetae_Con = (SMO.i16_Thetae + SMO.Comp + 3600)%3600; //角度补偿
}

static __INLINE void SMO_Thetae_Delay(void)   
{
	SMO.i16_Thetae_SMO_Delay = (Motor.info.i16_Thetae - SMO.i16_Thetae+3600)%3600;
	if(SMO.i16_Thetae_SMO_Delay>2000)
	{
		SMO.i16_Thetae_SMO_Delay = SMO.i16_Thetae_SMO_Delay - 3600;
	}
}

static __INLINE void SMOIF_Thetae_Delay(void)   
{
	SMO.i16_Thetae_SMOIF_Delay = (SMO.i16_Thetae_Con - Motor.info.i16_Thetae_IF + 3600 ) %3600;
	
	if(SMO.i16_Thetae_SMOIF_Delay>2000)
	{
		SMO.i16_Thetae_SMOIF_Delay = SMO.i16_Thetae_SMOIF_Delay - 3600;
	}
	
	
}

static __INLINE void Data_Record(void)
{
	if(GlobalVar.Record_Flag == 1&&GlobalVar.Record_Cnt<Record_Length)
	{
		GlobalVar.Record[0][GlobalVar.Record_Cnt]=Motor.info.i16_Ua;
		GlobalVar.Record[1][GlobalVar.Record_Cnt]=Motor.info.i16_Ub;
		GlobalVar.Record[2][GlobalVar.Record_Cnt]=SMO.i32_Ialpha;  
		GlobalVar.Record[3][GlobalVar.Record_Cnt]=SMO.i32_Ibeta;
		GlobalVar.Record_Cnt++;		
	}
}

static __INLINE void IF_SMO_Switch(void)
{
	SMOIF_Thetae_Delay(); 
	SMO_IF_Filter();
    SMO.i16_Thetae_SMOIF_Delay_Fil = (SMO.i16_Thetae_SMOIF_Delay_Fil + 3600)%3600;
	SMO.Id_temp = Motor.cmd.i16_Iq*SIN(SMO.i16_Thetae_SMOIF_Delay_Fil)>>15;   //电流环之后的d轴给定初始值
	Motor.cmd.i16_Id = SMO.Id_temp;

	SMO.Iq_temp = Motor.cmd.i16_Iq*COS(SMO.i16_Thetae_SMOIF_Delay_Fil)>>15;   //之后转速环输出的初值
	Motor.cmd.i16_Iq = SMO.Iq_temp;					
	output = SMO.Iq_temp;                    //增量式使用

	SMO.Ud_temp = (Motor.info.i16_Ud*COS(SMO.i16_Thetae_SMOIF_Delay_Fil)>>15) + (Motor.info.i16_Uq*SIN(SMO.i16_Thetae_SMOIF_Delay_Fil)>>15);   //之后d轴电流环的初值
	Motor.info.i16_Ud = SMO.Ud_temp;
	Output_Id  = SMO.Ud_temp;

	SMO.Uq_temp = (-Motor.info.i16_Ud*SIN(SMO.i16_Thetae_SMOIF_Delay_Fil)>>15) + (Motor.info.i16_Uq*COS(SMO.i16_Thetae_SMOIF_Delay_Fil)>>15);  //之后q轴电流环的初值
	Motor.info.i16_Uq = SMO.Uq_temp;
	Output_Iq = SMO.Uq_temp;
	
	SMO.Switch = 1;                 //双dq切换完成
}

#endif
