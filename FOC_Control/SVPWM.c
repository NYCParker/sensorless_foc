#include "SVPWM.h"

TSVPWM16_7  SVPWM16;

void SVPWM16_7(int16_t V_alpha, int16_t V_beta)
{
	SVPWM16.Vref1	=	V_beta;
	SVPWM16.Vref2	=	(short)(((long)V_alpha*(long)28378)>>15)				-(short)(((long)V_beta*(long)16384)>>15);
	SVPWM16.Vref3	=	0	-	(short)(((long)V_alpha*(long)28378)>>15)		-(short)(((long)V_beta*(long)16384)>>15);
	

	//Uq max:0x7FFF;
	SVPWM16.X =	(short)(((long)SVPWM16.Vref1*(long)SVPWM16.PWM_Period)>>15);
	SVPWM16.Y =	0-(short)(((long)SVPWM16.Vref3*(long)SVPWM16.PWM_Period)>>15);
	SVPWM16.Z =	0-(short)(((long)SVPWM16.Vref2*(long)SVPWM16.PWM_Period)>>15);


	if(SVPWM16.Vref1>0){SVPWM16.A=1;}
	else		{SVPWM16.A=0;}
	if(SVPWM16.Vref2>0){SVPWM16.B=2;}
	else		{SVPWM16.B=0;}
	if(SVPWM16.Vref3>0){SVPWM16.C=4;}
	else		{SVPWM16.C=0;}
	SVPWM16.Sector_Old	=	SVPWM16.Sector;
	SVPWM16.Sector=SVPWM16.A+SVPWM16.B+SVPWM16.C;  //判断扇区


	switch(SVPWM16.Sector)          //根据扇区计算时间
	{
		case 1:
 			SVPWM16.t1=SVPWM16.Z;
			SVPWM16.t2=SVPWM16.Y;
			//SVPWM16.Sector_NUM=2;
					break;
		case 2:
			SVPWM16.t1=SVPWM16.Y;
			SVPWM16.t2=0-SVPWM16.X;
			//SVPWM16.Sector_NUM=6;
					break;
		case 3:
			SVPWM16.t1=0-SVPWM16.Z;
			SVPWM16.t2=SVPWM16.X;
			//SVPWM16.Sector_NUM=1;
					break;
		case 4:
			SVPWM16.t1=0-SVPWM16.X;
			SVPWM16.t2=SVPWM16.Z;
			//SVPWM16.Sector_NUM=4;
					break;
		case 5:
			SVPWM16.t1=SVPWM16.X;
			SVPWM16.t2=0-SVPWM16.Y;
			//SVPWM16.Sector_NUM=3;
					break;
		case 6:
			SVPWM16.t1=0-SVPWM16.Y;
			SVPWM16.t2=0-SVPWM16.Z;
			//SVPWM16.Sector_NUM=5;
					break;

		default:
			SVPWM16.t1=0;
			SVPWM16.t2=0;
					break;
	}
	SVPWM16.t_Sum		=			SVPWM16.t1 + SVPWM16.t2;


	if(SVPWM16.t_Sum>SVPWM16.PWM_Period)    //过调制处理  
	{		
		SVPWM16.t1=(long)(SVPWM16.PWM_Period*SVPWM16.t1)/SVPWM16.t_Sum;
		SVPWM16.t2=SVPWM16.PWM_Period - SVPWM16.t1;
//		Overdose_flag++;
	}

	SVPWM16.taon=(SVPWM16.PWM_Period-SVPWM16.t1-SVPWM16.t2)>>1;
	SVPWM16.tbon=SVPWM16.taon+SVPWM16.t1;	
	SVPWM16.tcon=SVPWM16.tbon+SVPWM16.t2;							//low side ON_TIME :tcon>tbon>taon		


	switch(SVPWM16.Sector)  //根据扇区计算开关管占空比
	{
		case 1:
			SVPWM16.PDC_U=SVPWM16.tbon;
			SVPWM16.PDC_V=SVPWM16.taon;
			SVPWM16.PDC_W=SVPWM16.tcon;

					break;
		case 2:
			SVPWM16.PDC_U=SVPWM16.taon;
			SVPWM16.PDC_V=SVPWM16.tcon;
			SVPWM16.PDC_W=SVPWM16.tbon;

					break;
		case 3:
			SVPWM16.PDC_U=SVPWM16.taon;
			SVPWM16.PDC_V=SVPWM16.tbon;
			SVPWM16.PDC_W=SVPWM16.tcon;

					break;
		case 4:
			SVPWM16.PDC_U=SVPWM16.tcon;
			SVPWM16.PDC_V=SVPWM16.tbon;
			SVPWM16.PDC_W=SVPWM16.taon;

					break;
		case 5:
			SVPWM16.PDC_U=SVPWM16.tcon;
			SVPWM16.PDC_V=SVPWM16.taon;
			SVPWM16.PDC_W=SVPWM16.tbon;

					break;
		case 6:
			SVPWM16.PDC_U=SVPWM16.tbon;
			SVPWM16.PDC_V=SVPWM16.tcon;
			SVPWM16.PDC_W=SVPWM16.taon;

					break;

		default:
			SVPWM16.PDC_U=SVPWM16.PWM_Period;
			SVPWM16.PDC_V=SVPWM16.PWM_Period;
			SVPWM16.PDC_W=SVPWM16.PWM_Period;
					break;
	}
}
