/* 
上位机协议
*/

#include "Agreement.h"
#include "SVPWM.h"
#include "DAC.h"
#include "UART.h"
#include <xmc_uart.h>
#include "VAR_Initialize.h"
#include "PI_Control.h"

static uint8_t send_cnt = 50;
uint8_t send_buf[32];           //发送buffer
uint8_t Uart_Send_Flag = 0;
uint16_t Rec_Sta = 0;
uint8_t Uart_Buffer[REC_LENGTH];//接收buffer
uint8_t hand_shake_Flag = 0;    //握手
uint8_t uart_state_flag = 0;

uint16_t Current_Cmd_Iq = 400;

//串口对电脑
/*******************communication App*************************/ 
uint8_t CRC_check(uint8_t *Buf, uint8_t len)
{
	uint32_t sum=0;
	uint8_t i = 0;
	for(i = 0; i < len; i++)
	{
		sum += Buf[i];
	}
	return (uint8_t)(sum);
}


void Communication_App(void)          
{
	
	uint16_t IqKp_Temp,IqKi_Temp;
    uint16_t ixxxx;
	
	if(--send_cnt == 0)
	{
		if(GlobalVar.Debug_Switch == 0)
		{
		IqKp_Temp = (uint16_t)(PI_Speed.flo_kp*10000);
		IqKi_Temp = (uint16_t)(PI_Speed.flo_ki*10000);
		
		send_buf[0] = 0xa1;
		send_buf[1] = Motor.flag.state; 
		send_buf[2] = (uint8_t)SMO.i32_Speed_ASMO_Fil;       //ch1
		send_buf[3] = (uint8_t)(SMO.i32_Speed_ASMO_Fil>>8);
//		send_buf[2] = (uint8_t)Motor.info.ui16_Speed;       //ch1
//		send_buf[3] = (uint8_t)(Motor.info.ui16_Speed>>8);
		send_buf[4] = (uint8_t)Motor.info.i16_Id;
		send_buf[5] = (uint8_t)(Motor.info.i16_Id>>8);
		send_buf[6] = (uint8_t)Motor.info.i16_Iq;
		send_buf[7] = (uint8_t)(Motor.info.i16_Iq>>8);
		send_buf[8] = (uint8_t)GlobalVar.Start_Record;
		send_buf[9] = (uint8_t)(GlobalVar.Start_Record>>8);		
	    send_buf[10] = (uint8_t)GlobalVar.Send_Record;
		send_buf[11] = (uint8_t)(GlobalVar.Send_Record>>8);			
		send_buf[12] = (uint8_t)GlobalVar.Lock_Time;
		send_buf[13] = (uint8_t)(GlobalVar.Lock_Time>>8);		
	    send_buf[14] = (uint8_t)SMO.i16_k;
		send_buf[15] = (uint8_t)(SMO.i16_k>>8);	
		send_buf[16] = (uint8_t)(Motor.cmd.i16_Speed);
		send_buf[17] = (uint8_t)((Motor.cmd.i16_Speed)>>8);
		send_buf[18] = (uint8_t)GlobalVar.Debug_Switch;
		send_buf[19] = (uint8_t)(GlobalVar.Debug_Switch>>8);
		send_buf[20] = (uint8_t)Motor.info.i16_Uq;
		send_buf[21] = (uint8_t)(Motor.info.i16_Uq>>8);			    //ch3
		send_buf[22] = (uint8_t)Motor.info.i16_Id ;					//ch3
		send_buf[23] = (uint8_t)(Motor.info.i16_Id >>8);
		send_buf[24] = (uint8_t)Motor.info.i16_Iq;					//ch4
		send_buf[25] = (uint8_t)(Motor.info.i16_Iq>>8);
		send_buf[26] = 0;
		send_buf[27] = 0;
		send_buf[28] = 0;
		send_buf[29] = CRC_check(send_buf,29);
		send_buf[30] = 0x78;
		send_buf[31] = 0x56;
	    }
		else if(GlobalVar.Debug_Switch == 1)
		{
			ixxxx = (uint16_t)Motor.cmd.i16_Iq;
		IqKp_Temp = (uint16_t)(PI_Iq.flo_kp*10000);
		IqKi_Temp = (uint16_t)(PI_Iq.flo_ki*10000);
		
		send_buf[0] = 0xa1;
		send_buf[1] = Motor.flag.state; 
		send_buf[2] = (uint8_t)SMO.i32_Speed_ASMO_Fil;       //ch1
		send_buf[3] = (uint8_t)(SMO.i32_Speed_ASMO_Fil>>8);
//		send_buf[2] = (uint8_t)Motor.info.ui16_Speed;       //ch1
//		send_buf[3] = (uint8_t)(Motor.info.ui16_Speed>>8);
		send_buf[4] = (uint8_t)Motor.info.i16_Id;
		send_buf[5] = (uint8_t)(Motor.info.i16_Id>>8);
		send_buf[6] = (uint8_t)Motor.info.i16_Iq;
		send_buf[7] = (uint8_t)(Motor.info.i16_Iq>>8);
		send_buf[8] = (uint8_t)IqKp_Temp;
		send_buf[9] = (uint8_t)(IqKp_Temp>>8);		
	    send_buf[10] = (uint8_t)IqKi_Temp;
		send_buf[11] = (uint8_t)(IqKi_Temp>>8);			
		send_buf[12] = (uint8_t)808;
		send_buf[13] = (uint8_t)(808>>8);		
	    send_buf[14] = (uint8_t)PI_Iq.i32_output_limit;
		send_buf[15] = (uint8_t)(PI_Iq.i32_output_limit>>8);	
		send_buf[16] = (uint8_t)(ixxxx);
		send_buf[17] = (uint8_t)((ixxxx)>>8);
		send_buf[18] = (uint8_t)GlobalVar.Debug_Switch;
		send_buf[19] = (uint8_t)(GlobalVar.Debug_Switch>>8);
		send_buf[20] = (uint8_t)Motor.info.i16_Uq;
		send_buf[21] = (uint8_t)(Motor.info.i16_Uq>>8);			    //ch3
		send_buf[22] = (uint8_t)Motor.info.i16_Id ;					//ch3
		send_buf[23] = (uint8_t)(Motor.info.i16_Id >>8);
		send_buf[24] = (uint8_t)Motor.info.i16_Iq;					//ch4
		send_buf[25] = (uint8_t)(Motor.info.i16_Iq>>8);
		send_buf[26] = 0;
		send_buf[27] = 0;
		send_buf[28] = 0;
		send_buf[29] = CRC_check(send_buf,29);
		send_buf[30] = 0x78;
		send_buf[31] = 0x56;
	    }
		else if(GlobalVar.Debug_Switch == 2)
		{
		IqKp_Temp = (uint16_t)(PI_Speed.flo_kp*10000);
		IqKi_Temp = (uint16_t)(PI_Speed.flo_ki*10000);
		
		send_buf[0] = 0xa1;
		send_buf[1] = Motor.flag.state; 
		send_buf[2] = (uint8_t)SMO.i32_Speed_ASMO_Fil;       //ch1
		send_buf[3] = (uint8_t)(SMO.i32_Speed_ASMO_Fil>>8);
//		send_buf[2] = (uint8_t)Motor.info.ui16_Speed;       //ch1
//		send_buf[3] = (uint8_t)(Motor.info.ui16_Speed>>8);
		send_buf[4] = (uint8_t)Motor.info.i16_Id;
		send_buf[5] = (uint8_t)(Motor.info.i16_Id>>8);
		send_buf[6] = (uint8_t)Motor.info.i16_Iq;
		send_buf[7] = (uint8_t)(Motor.info.i16_Iq>>8);
		send_buf[8] = (uint8_t)IqKp_Temp;
		send_buf[9] = (uint8_t)(IqKp_Temp>>8);		
	    send_buf[10] = (uint8_t)IqKi_Temp;
		send_buf[11] = (uint8_t)(IqKi_Temp>>8);			
		send_buf[12] = (uint8_t)808;
		send_buf[13] = (uint8_t)(808>>8);		
	    send_buf[14] = (uint8_t)PI_Speed.i32_output_limit;
		send_buf[15] = (uint8_t)(PI_Speed.i32_output_limit>>8);	
		send_buf[16] = (uint8_t)(Motor.cmd.i16_Speed);
		send_buf[17] = (uint8_t)((Motor.cmd.i16_Speed)>>8);
		send_buf[18] = (uint8_t)GlobalVar.Debug_Switch;
		send_buf[19] = (uint8_t)(GlobalVar.Debug_Switch>>8);
		send_buf[20] = (uint8_t)Motor.info.i16_Uq;
		send_buf[21] = (uint8_t)(Motor.info.i16_Uq>>8);			    //ch3
		send_buf[22] = (uint8_t)Motor.info.i16_Id ;					//ch3
		send_buf[23] = (uint8_t)(Motor.info.i16_Id >>8);
		send_buf[24] = (uint8_t)Motor.info.i16_Iq;					//ch4
		send_buf[25] = (uint8_t)(Motor.info.i16_Iq>>8);
		send_buf[26] = 0;
		send_buf[27] = 0;
		send_buf[28] = 0;
		send_buf[29] = CRC_check(send_buf,29);
		send_buf[30] = 0x78;
		send_buf[31] = 0x56;
	    }		
		else if(GlobalVar.Debug_Switch == 3)
		{
		IqKp_Temp = (uint16_t)(PI_SMO.flo_kp*10000);
		IqKi_Temp = (uint16_t)(PI_SMO.flo_ki*10000);
		
		send_buf[0] = 0xa1;
		send_buf[1] = Motor.flag.state; 
		send_buf[2] = (uint8_t)SMO.i32_Speed_ASMO_Fil;       //ch1
		send_buf[3] = (uint8_t)(SMO.i32_Speed_ASMO_Fil>>8);
//		send_buf[2] = (uint8_t)Motor.info.ui16_Speed;       //ch1
//		send_buf[3] = (uint8_t)(Motor.info.ui16_Speed>>8);
		send_buf[4] = (uint8_t)Motor.info.i16_Id;
		send_buf[5] = (uint8_t)(Motor.info.i16_Id>>8);
		send_buf[6] = (uint8_t)Motor.info.i16_Iq;
		send_buf[7] = (uint8_t)(Motor.info.i16_Iq>>8);
		send_buf[8] = (uint8_t)IqKp_Temp;
		send_buf[9] = (uint8_t)(IqKp_Temp>>8);		
	    send_buf[10] = (uint8_t)IqKi_Temp;
		send_buf[11] = (uint8_t)(IqKi_Temp>>8);			
		send_buf[12] = (uint8_t)SMO.i32_PLL_Error;
		send_buf[13] = (uint8_t)(SMO.i32_PLL_Error>>8);		
	    send_buf[14] = (uint8_t)SMO_Output;
		send_buf[15] = (uint8_t)(SMO_Output>>8);	
		send_buf[16] = (uint8_t)(808);
		send_buf[17] = (uint8_t)((808)>>8);
		send_buf[18] = (uint8_t)GlobalVar.Debug_Switch;
		send_buf[19] = (uint8_t)(GlobalVar.Debug_Switch>>8);
		send_buf[20] = (uint8_t)Motor.info.i16_Uq;
		send_buf[21] = (uint8_t)(Motor.info.i16_Uq>>8);			    //ch3
		send_buf[22] = (uint8_t)Motor.info.i16_Id ;					//ch3
		send_buf[23] = (uint8_t)(Motor.info.i16_Id >>8);
		send_buf[24] = (uint8_t)Motor.info.i16_Iq;					//ch4
		send_buf[25] = (uint8_t)(Motor.info.i16_Iq>>8);
		send_buf[26] = 0;
		send_buf[27] = 0;
		send_buf[28] = 0;
		send_buf[29] = CRC_check(send_buf,29);
		send_buf[30] = 0x78;
		send_buf[31] = 0x56;
	    }	
		else if(GlobalVar.Debug_Switch == 4)
		{
		IqKp_Temp = (uint16_t)(PI_Speed.flo_kp*10000);
		IqKi_Temp = (uint16_t)(PI_Speed.flo_ki*10000);
		
		send_buf[0] = 0xa1;
		send_buf[1] = Motor.flag.state; 
		send_buf[2] = (uint8_t)SMO.i32_Speed_ASMO_Fil;       //ch1
		send_buf[3] = (uint8_t)(SMO.i32_Speed_ASMO_Fil>>8);
//		send_buf[2] = (uint8_t)Motor.info.ui16_Speed;       //ch1
//		send_buf[3] = (uint8_t)(Motor.info.ui16_Speed>>8);
		send_buf[4] = (uint8_t)Motor.info.i16_Id;
		send_buf[5] = (uint8_t)(Motor.info.i16_Id>>8);
		send_buf[6] = (uint8_t)Motor.info.i16_Iq;
		send_buf[7] = (uint8_t)(Motor.info.i16_Iq>>8);
		send_buf[8] = (uint8_t)SMO.ui16_F;
		send_buf[9] = (uint8_t)(SMO.ui16_F>>8);		
	    send_buf[10] = (uint8_t)SMO.ui16_G;
		send_buf[11] = (uint8_t)(SMO.ui16_G>>8);			
		send_buf[12] = (uint8_t)(int)(SMO.ui16_kslide);
		send_buf[13] = (uint8_t)((int)(SMO.ui16_kslide)>>8);		
	    send_buf[14] = (uint8_t)SMO.i16_k;
		send_buf[15] = (uint8_t)(SMO.i16_k>>8);	
		send_buf[16] = (uint8_t)(Motor.flag.Switch);
		send_buf[17] = (uint8_t)((Motor.flag.Switch)>>8);
		send_buf[18] = (uint8_t)GlobalVar.Debug_Switch;
		send_buf[19] = (uint8_t)(GlobalVar.Debug_Switch>>8);
		send_buf[20] = (uint8_t)Motor.info.i16_Uq;
		send_buf[21] = (uint8_t)(Motor.info.i16_Uq>>8);			    //ch3
		send_buf[22] = (uint8_t)Motor.info.i16_Id ;					//ch3
		send_buf[23] = (uint8_t)(Motor.info.i16_Id >>8);
		send_buf[24] = (uint8_t)Motor.info.i16_Iq;					//ch4
		send_buf[25] = (uint8_t)(Motor.info.i16_Iq>>8);
		send_buf[26] = 0;
		send_buf[27] = 0;
		send_buf[28] = 0;
		send_buf[29] = CRC_check(send_buf,29);
		send_buf[30] = 0x78;
		send_buf[31] = 0x56;
	    }
		else if(GlobalVar.Debug_Switch == 5)
		{
		IqKp_Temp = (uint16_t)(PI_Speed.flo_kp*10000);
		IqKi_Temp = (uint16_t)(PI_Speed.flo_ki*10000);
		
		send_buf[0] = 0xa1;
		send_buf[1] = Motor.flag.state; 
		send_buf[2] = (uint8_t)SMO.i32_Speed_ASMO_Fil;       //ch1
		send_buf[3] = (uint8_t)(SMO.i32_Speed_ASMO_Fil>>8);
//		send_buf[2] = (uint8_t)Motor.info.ui16_Speed;       //ch1
//		send_buf[3] = (uint8_t)(Motor.info.ui16_Speed>>8);
		send_buf[4] = (uint8_t)Motor.info.i16_Id;
		send_buf[5] = (uint8_t)(Motor.info.i16_Id>>8);
		send_buf[6] = (uint8_t)Motor.info.i16_Iq;
		send_buf[7] = (uint8_t)(Motor.info.i16_Iq>>8);
		send_buf[8] = (uint8_t)Motor.info.i16_Thetae_Inc;
		send_buf[9] = (uint8_t)(Motor.info.i16_Thetae_Inc>>8);		
	    send_buf[10] = (uint8_t)GlobalVar.Thetae_Inc_Max;
		send_buf[11] = (uint8_t)(GlobalVar.Thetae_Inc_Max>>8);			
		send_buf[12] = (uint8_t)GlobalVar.Thetae_InTerval;
		send_buf[13] = (uint8_t)(GlobalVar.Thetae_InTerval>>8);		
	    send_buf[14] = (uint8_t)SMO.Comp;
		send_buf[15] = (uint8_t)(SMO.Comp>>8);	
		send_buf[16] = (uint8_t)(GlobalVar.Thetae_IFInc_Interval);
		send_buf[17] = (uint8_t)((GlobalVar.Thetae_IFInc_Interval)>>8);
		send_buf[18] = (uint8_t)GlobalVar.Debug_Switch;
		send_buf[19] = (uint8_t)(GlobalVar.Debug_Switch>>8);
		send_buf[20] = (uint8_t)Motor.info.i16_Uq;
		send_buf[21] = (uint8_t)(Motor.info.i16_Uq>>8);			    //ch3
		send_buf[22] = (uint8_t)Motor.info.i16_Id ;					//ch3
		send_buf[23] = (uint8_t)(Motor.info.i16_Id >>8);
		send_buf[24] = (uint8_t)Motor.info.i16_Iq;					//ch4
		send_buf[25] = (uint8_t)(Motor.info.i16_Iq>>8);
		send_buf[26] = 0;
		send_buf[27] = 0;
		send_buf[28] = 0;
		send_buf[29] = CRC_check(send_buf,29);
		send_buf[30] = 0x78;
		send_buf[31] = 0x56;
	    }
		Uart_Send_Flag = 1;
		send_cnt = 50;
	}
}

void Virtual_Scope(void)          
{
	int Record0;
	int Record1;
	int Record2;
	int Record3;

	if(--send_cnt == 0)
	{
		Record0 = GlobalVar.Record[0][GlobalVar.Data_Row];
		Record1 = GlobalVar.Record[1][GlobalVar.Data_Row];
		Record2 = GlobalVar.Record[2][GlobalVar.Data_Row];
		Record3 = GlobalVar.Record[3][GlobalVar.Data_Row];
		
		GlobalVar.Data_Row++;
		
		if(GlobalVar.Data_Row == Record_Length)
		{
			GlobalVar.Data_Row = 0;
			GlobalVar.Record_Flag = 3;
		}		
		
		send_buf[0] = 0xa1;
		send_buf[1] = Motor.flag.state; 
		send_buf[2] = (uint8_t)Record0;                   //ch1
		send_buf[3] = (uint8_t)(Record0>>8);
		send_buf[20] = (uint8_t)Record1;
		send_buf[21] = (uint8_t)(Record1>>8);			        //ch3
		send_buf[22] = (uint8_t)Record2 ;	//ch3
		send_buf[23] = (uint8_t)(Record2>>8);
		send_buf[24] = (uint8_t)Record3;					    //ch4
		send_buf[25] = (uint8_t)(Record3>>8);
		send_buf[26] = 0;
		send_buf[27] = 0;
		send_buf[28] = 0;
		send_buf[29] = CRC_check(send_buf,29);
		send_buf[30] = 0x78;
		send_buf[31] = 0x56;
		Uart_Send_Flag = 1;
		send_cnt = 50;
	}
}

//电脑对串口
void Uart_Rec(void)                     
{
	int16_t data16 = 0;
	float IKp_Tempsend;
	float IKi_Tempsend;
	
	if(Rec_Sta&0x8000)
	{
		if(((Rec_Sta&0x3fff) == 6)&&(Uart_Buffer[0]==0xa1))//8-2
		{
			data16 = (int16_t)(Uart_Buffer[2])|(Uart_Buffer[3]<<8);
			
			if(GlobalVar.Debug_Switch == 0)
			{
			switch(Uart_Buffer[1])
			{
				case 0: hand_shake_Flag = data16;         //握手
						break;      
				
				case 1: if(data16 > 1)                    //启动标志
								{
									data16 = 0;
								}
						uart_state_flag = data16;
						break;        
								
				case 2: Motor.cmd.i16_Speed= data16;      //参数设置1
						break;        
								
				case 3:	GlobalVar.Start_Record = data16;       	  //参数设置2						
						break;
							
				case 4: GlobalVar.Send_Record = data16;            //参数设置3
						break;
								
   			    case 5: GlobalVar.Lock_Time = data16;//参数设置4         
						break;
								
			    case 6: SMO.i16_k = data16; //参数设置5
						break;
						
			    case 7: GlobalVar.Debug_Switch = data16;  //参数设置6
				        break;		
								
				default:break;				
			}
		   }
			
		   else if(GlobalVar.Debug_Switch == 1)
			{
			switch(Uart_Buffer[1])
			{
				case 0: hand_shake_Flag = data16;         //握手
						break;      
				
				case 1: if(data16 > 1)                    //启动标志
								{
									data16 = 0;
								}
						uart_state_flag = data16;
						break;        
								
				case 2: //Motor.cmd.i16_Iq= data16;      //参数设置1
						Current_Cmd_Iq = data16;
						//Motor.cmd.IF_Speed8e533 = data16;
						break;        
								
				case 3:	IKp_Tempsend = data16;       	  //参数设置2
					    PI_Iq.flo_kp=IKp_Tempsend/10000;
                        PI_Id.flo_kp=IKp_Tempsend/10000;								
						break;
							
				case 4: IKi_Tempsend = data16;            //参数设置3
						PI_Iq.flo_ki=IKi_Tempsend/10000;
						PI_Id.flo_ki=IKi_Tempsend/10000;
						break;
								
   			    //case 5: SMO.ui16_kslide = data16;//参数设置4         
				//		break;
								
			    case 6: PI_Iq.i32_output_limit = data16; //参数设置5
				        PI_Id.i32_output_limit = data16;
						break;
						
			    case 7: GlobalVar.Debug_Switch = data16;  //参数设置6
				        break;		
								
				default:break;				
			}
		 }
			else if(GlobalVar.Debug_Switch == 2)
			{
			switch(Uart_Buffer[1])
			{
				case 0: hand_shake_Flag = data16;         //握手
						break;      
				
				case 1: if(data16 > 1)                    //启动标志
								{
									data16 = 0;
								}
						uart_state_flag = data16;
						break;        
								
				case 2: Motor.cmd.i16_Speed= data16;      //参数设置1
						//Motor.cmd.IF_Speed8e533 = data16;
						break;        
								
				case 3:	IKp_Tempsend = data16;       	  //参数设置2
					    PI_Speed.flo_kp=IKp_Tempsend/10000;							
						break;
							
				case 4: IKi_Tempsend = data16;            //参数设置3
						PI_Speed.flo_ki=IKi_Tempsend/10000;
						break;
								
   			  //  case 5: SMO.ui16_kslide = data16;//参数设置4         
				//		break;
								
			    case 6: PI_Speed.i32_output_limit = data16; //参数设置5
						break;
						
			    case 7: GlobalVar.Debug_Switch = data16;  //参数设置6
				        break;		
								
				default:break;				
			}
		 }			
			else if(GlobalVar.Debug_Switch == 3)
			{
			switch(Uart_Buffer[1])
			{
				case 0: hand_shake_Flag = data16;         //握手
						break;      
				
				case 1: if(data16 > 1)                    //启动标志
								{
									data16 = 0;
								}
						uart_state_flag = data16;
						break;        
								
				//case 2: Motor.cmd.i16_Speed= data16;      //参数设置1
						//Motor.cmd.IF_Speed8e533 = data16;
			//			break;        
								
				case 3:	IKp_Tempsend = data16;       	  //参数设置2
					    PI_SMO.flo_kp=IKp_Tempsend/10000;							
						break;
							
				case 4: IKi_Tempsend = data16;            //参数设置3
						PI_SMO.flo_ki=IKi_Tempsend/10000;
						break;
								
   			    case 5: SMO.i32_PLL_Error = data16;       //参数设置4         
						break;
								
			    case 6: SMO_Output = data16; //参数设置5
						break;
						
			    case 7: GlobalVar.Debug_Switch = data16;  //参数设置6
				        break;		
								
				default:break;				
			}
		 }			
			else if(GlobalVar.Debug_Switch == 4)
			{
			switch(Uart_Buffer[1])
			{
				case 0: hand_shake_Flag = data16;         //握手
						break;      
				
				case 1: if(data16 > 1)                    //启动标志
								{
									data16 = 0;
								}
						uart_state_flag = data16;
						break;        
								
				case 2: Motor.flag.Switch = data16;      //参数设置1
						break;        
								
				case 3:	SMO.ui16_F = data16;       	  //参数设置2							
						break;
							
				case 4: SMO.ui16_G = data16;            //参数设置3
						break;
								
   			    case 5: SMO.ui16_kslide = data16;//参数设置4         
						break;
								
			    case 6: SMO.i16_k = data16; //参数设置5
						break;
						
			    case 7: GlobalVar.Debug_Switch = data16;  //参数设置6
				        break;		
								
				default:break;				
			}
		 }			
			else if(GlobalVar.Debug_Switch == 5)
			{
			switch(Uart_Buffer[1])
			{
				case 0: hand_shake_Flag = data16;         //握手
						break;      
				
				case 1: if(data16 > 1)                    //启动标志
								{
									data16 = 0;
								}
						uart_state_flag = data16;
						break;        
		
				case 2:	GlobalVar.Thetae_IFInc_Interval = data16;      //参数设置2							
						break;
							
				case 3: Motor.info.i16_Thetae_Inc = data16;            //参数设置3
						break;
								
   			    case 4: GlobalVar.Thetae_Inc_Max = data16;             //参数设置4         
						break;
								
			    case 5: GlobalVar.Thetae_InTerval = data16;            //参数设置5
						break;
				case 6: SMO.Comp = data16;      //参数设置1
					    break;  
						
			    case 7: GlobalVar.Debug_Switch = data16;               //参数设置6
				        break;		
								
				default:break;				
			}
		 }			
			
		 
	  }	
		Rec_Sta = 0;
	}
}

void Serial_Communication(void)
{
	if(Uart_Send_Flag)
	{
		Uart_Send_Flag = 0;
		UART_SendBuffer(send_buf, 32);
	}
	Uart_Rec();	
}

//串口中断
void USIC0_0_IRQHandler(void)
{
		uint8_t Res;
    if ((USIC0_CH1->PSR_ASCMode & 0x8000)||(USIC0_CH1->PSR_ASCMode & 0x4000))     // Alternate receive interrupt flag
    {
        USIC0_CH1->PSCR   |= 0x8000;         // clear PSR_AIF
				USIC0_CH1->PSCR   |= 0x4000;            // clear PSR_RIF   
				Res = USIC0_CH1->RBUF;	
				if((Rec_Sta&0x8000)==0)
				{
					if(Rec_Sta&0x4000)
					{
						if(Res!=0x12)
							Rec_Sta=0;//接收错误,重新开始
						else 
							Rec_Sta|=0x8000;	//接收完成 
					}
					else 
					{	
						if(Res==0x34)
							Rec_Sta|=0x4000;
						else
						{
							Uart_Buffer[Rec_Sta&0X3FFF]=Res ;
							Rec_Sta++;
							if(Rec_Sta>(REC_LENGTH-1))Rec_Sta=0;//接收数据错误,重新开始接收	  
						}		 
					}
				}
    }
}
