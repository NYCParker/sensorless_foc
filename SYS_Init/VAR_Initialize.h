#ifndef _VAR_Initialize_h
#define _VAR_Initialize_h

#include <XMC4400.h>

#define Config_Hardware_Kitboard 1
#define Sensorless_Mode 1

#define REC_LENGTH  			200 
#define Record_Length  			3600
#define d_Fil_Length            8000     
#define d1_Fil_Length           2000
#define SMO_Speed_Length        4000 
#define Current_Max             5000

#define Control_Loop_Mode  1    //1:电流单闭环 2：双闭环

typedef enum
{
	Motor_Stop,
	Motor_Lock,
	Motor_Start,
	Motor_Run,
}MotorState;

typedef struct //一些电机常数
{
	uint16_t ui16_Iu_ADCRef;
	uint16_t ui16_Iv_ADCRef;
	uint16_t ui16_Iw_ADCRef;  //上电先校正
} MOTORConstant; 

typedef struct //一些电机标志
{
	MotorState state;
	uint8_t Lock_done;
	uint8_t Switch;
} MOTORFlag; 

typedef struct //电机实时变量
{	
	uint16_t ui16_Iu_ADC;      
	uint16_t ui16_Iv_ADC;
	uint16_t ui16_Iw_ADC;
	uint16_t ui16_Uu_ADC;      
	uint16_t ui16_Uv_ADC;
	uint16_t ui16_Uw_ADC;
	int16_t  i16_Iu;        
	int16_t  i16_Iv;
	int16_t  i16_Iw;
	int16_t  i16_Iw_t;
	int16_t i16_Iq;
	int16_t i16_Id; 
	int16_t i16_Ud;        
	int16_t i16_Uq;         
	int16_t i16_Ua; 
	int16_t i16_Ub; 
	
	//无感用不到
	int16_t i16_Thetae;
	int16_t  i16_T_Timer_Count;
	uint16_t ui16_T_Timer_Count_Handle;
	uint16_t ui16_Speed;
	int16_t i16_Thetae_IF_Delay;
	int16_t i16_Thetae_IF_Delay_Fil;
	//
	
	int16_t i16_Thetae_Inc;
	int16_t i16_Thetae_IF;
	
	
} MOTORInfo; 

typedef struct 
{
	int16_t i16_Iq;
	int16_t i16_Id;
	uint8_t State;
	int16_t i16_Speed; 
} MOTORCommand; 

typedef struct 
{
} MOTORTest; 

typedef struct 
{
	MOTORCommand cmd;
	MOTORInfo info;
	MOTORConstant cst;
	MOTORFlag flag;
	MOTORTest test;
} MOTORs; 

typedef struct   //累加计数值
{
	uint16_t ui16_Thetae_IFlock;
	uint16_t ui16_Thetae_IFInc_Interval;   //IF软起动间隔（phie_inc每隔多久增加1，根据控制周期设置）
	uint16_t ui16_Thetae_Interval;         //(phie每隔多久增加phie_inc)
	uint16_t ui16_Thetae_IFSwitch;
}COUNTs;

typedef struct 
{
	uint16_t R_Value; 
	
	uint8_t  Record_Flag;
    uint16_t Record_Cnt;
    int16_t  Record[4][Record_Length];
	uint16_t Data_Line;
	uint16_t Data_Row;
	
	uint8_t Start_Record;
	uint8_t Send_Record;
	
	uint16_t Thetae_IFInc_Interval; //每隔多少个62.5us I/F旋转速度加1
	uint8_t Thetae_Inc_Max;         //最大加到多少
	uint8_t Thetae_InTerval;        //每隔多少个62.5us I/F角度加Inc
	
	uint8_t  Debug_Switch;
	uint16_t Lock_Time;
} GlobalVars;

typedef struct   //SMO
{
	int32_t i32_Ialpha;          
	int32_t i32_Ibeta;
	int32_t i32_Ualpha;          
	int32_t i32_Ubeta;
	
	int32_t i32_Zalpha;
	int32_t i32_Zbeta;
	
	
 	int32_t i32_EstIalpha;          
	int32_t i32_EstIbeta; 
 	int32_t i32_Ialpha_Error;          
	int32_t i32_Ibeta_Error; 
	
	int32_t i32_Ealpha;
	int32_t i32_Ebeta;

	uint16_t ui16_G;
	uint16_t ui16_F;
	uint16_t ui16_kslide;
	uint16_t ui16_Kslf;	
	
	int32_t i32_Zmax;
	int16_t i32_Emax;
	
	int16_t i16_Thetae;
	int16_t i16_Thetae_Last;
	int16_t i16_Thetae_Con;
	
	int16_t i16_Thetae_SMO_Delay;
	int16_t i16_Thetae_SMO_Delay_Fil;
	
	int16_t i16_Thetae_SMOIF_Delay;
	int16_t i16_Thetae_SMOIF_Delay_Fil;	
	
	int32_t i32_PLL_Error;
	int16_t i16_Speed_Est;
	uint8_t Switch;
	
	int16_t Ud_temp;
	int16_t Uq_temp;        
	int16_t Id_temp;        
	int16_t Iq_temp;
	
	uint16_t  speedloop_count;
	
	int32_t i32_Speed_ASMO;
	int32_t i32_Speed_ASMO_Fil;
	int16_t i16_k;
	
	int16_t Comp;
}SMOs;

typedef struct   //float版本 增量式
{
	int32_t i32_e1;  //本次误差
	int32_t i32_e2; 
	float flo_kp;
	float flo_ki;
    int32_t i32_output;
	int32_t i32_output_limit;
}PIDs;

extern SMOs SMO;
extern PIDs PI_Iq,  PI_Id,  PI_SMO, PI_Speed;
extern COUNTs Count;
extern MOTORs Motor;
extern GlobalVars GlobalVar;
extern void Initialize_All_Var(void);
extern void Cur_Calibration(void);
#endif
