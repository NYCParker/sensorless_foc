#include "PI_Control.h"
#include "VAR_Initialize.h"

int32_t SMO_Output;

float P_output_pos = 0;
float I_output_pos = 0;
float I_Sum = 0;
float I_Max = 50000;
float output = 0;

float P_Inc_Iq;
float I_Inc_Iq;
float Inc_Iq;
float Output_Iq;

float P_Inc_Id;
float I_Inc_Id;
float Inc_Id;
float Output_Id;

void PI_SMO_PLL(void)
{
	float P_output_inc;
	float I_output_inc;
	
	PI_SMO.i32_e2 = PI_SMO.i32_e1;
	PI_SMO.i32_e1 = SMO.i32_PLL_Error; 
    
	P_output_inc = PI_SMO.flo_kp*(PI_SMO.i32_e1-PI_SMO.i32_e2);
	I_output_inc = PI_SMO.flo_ki*PI_SMO.i32_e1;
	
	SMO_Output = P_output_inc + I_output_inc;
	
	SMO.i16_Thetae = (SMO.i16_Thetae + SMO_Output + 3600)%3600;  	
}

void PI_Id_Loop(void)
{
	PI_Id.i32_e2 = PI_Id.i32_e1;
	PI_Id.i32_e1 = Motor.cmd.i16_Id - Motor.info.i16_Id; 
    
	P_Inc_Id = PI_Id.flo_kp*(PI_Id.i32_e1-PI_Id.i32_e2);
	I_Inc_Id = PI_Id.flo_ki*PI_Id.i32_e1;
	
	Inc_Id = P_Inc_Id + I_Inc_Id;
	
	Output_Id = Output_Id + Inc_Id;

    if(Output_Id >= PI_Id.i32_output_limit) 
	{Output_Id = PI_Id.i32_output_limit;}
    else if(Output_Id <= -PI_Id.i32_output_limit)
	{
		Output_Id = -PI_Id.i32_output_limit;
	}
	
	Motor.info.i16_Ud = (int)Output_Id;
}

void PI_Iq_Loop(void)
{
	PI_Iq.i32_e2 = PI_Iq.i32_e1;
	PI_Iq.i32_e1 = Motor.cmd.i16_Iq - Motor.info.i16_Iq; 
    
	P_Inc_Iq = PI_Iq.flo_kp*(PI_Iq.i32_e1-PI_Iq.i32_e2);
	I_Inc_Iq = PI_Iq.flo_ki*PI_Iq.i32_e1;
	
	Inc_Iq = P_Inc_Iq + I_Inc_Iq;
	
	Output_Iq = Output_Iq + Inc_Iq;
	
    if(Output_Iq >= PI_Iq.i32_output_limit) Output_Iq = PI_Iq.i32_output_limit;
    else if(Output_Iq <= -PI_Iq.i32_output_limit) Output_Iq = -PI_Iq.i32_output_limit;	
	
	Motor.info.i16_Uq = (int)Output_Iq;

}

void PI_Speed_Loop(void)
{
	float output_inc;
	float P_output_inc;
	float I_output_inc;
	
	PI_Speed.i32_e2 = PI_Speed.i32_e1;
    PI_Speed.i32_e1 = Motor.cmd.i16_Speed - SMO.i16_Speed_Est; 
	
	P_output_inc = PI_Speed.flo_kp*(PI_Speed.i32_e1-PI_Speed.i32_e2);
	I_output_inc = PI_Speed.flo_ki*PI_Speed.i32_e1;
	
	output_inc = P_output_inc + I_output_inc;
	output = output + output_inc;
	
    if(output >= PI_Speed.i32_output_limit) output = PI_Speed.i32_output_limit;
    else if(output <  -PI_Speed.i32_output_limit) output = -PI_Speed.i32_output_limit;
	
	Motor.cmd.i16_Iq = output;
}
