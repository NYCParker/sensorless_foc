#ifndef _Motor_Functions_h
#define _Motor_Functions_h

#include <XMC4400.h>

extern const int16_t sintable16[3600];

extern void Re_Constuct_3_Phase_Current(void);

extern void Iuvw_to_Idq(uint16_t anglee);
	
extern void Udq_to_UarphaUbeta(uint16_t anglee);

extern void Thetae_Lock_CCU8(void);
	
extern void Thetae_Inc_CCU8(void);    

extern void IF_Thetae_Delay(void);   

extern void ASMO(void);    
 
extern void SMO_Thetae_Delay(void);  

extern void SMOIF_Thetae_Delay(void);   

extern void Data_Record(void);

extern void IF_SMO_Switch(void);

#endif
