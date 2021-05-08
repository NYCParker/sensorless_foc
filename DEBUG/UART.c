
/* 
UART配置
*/

#include "UART.h"
#include <xmc_uart.h>
#include "xmc_gpio.h"

#define FDR_STEP	928
#define BRG_PDIV	58
#define BRG_DCTQ	15
#define BRG_PCTQ	0

uint32_t Read_KSCFG;

 void UART_Init(void)
{
	WR_REG(SCU_RESET->PRSET0, SCU_RESET_PRSET0_USIC0RS_Msk, SCU_RESET_PRSET0_USIC0RS_Pos, 1);   //复位USIC外设（上电即复位，这一步可忽略）
	WR_REG(SCU_RESET->PRCLR0, SCU_RESET_PRCLR0_USIC0RS_Msk, SCU_RESET_PRCLR0_USIC0RS_Pos, 1);   //解除复位
	
	USIC0_CH1->KSCFG =
				((0x1 << USIC_CH_KSCFG_MODEN_Pos) & USIC_CH_KSCFG_MODEN_Msk) |
		        ((0x1 << USIC_CH_KSCFG_BPMODEN_Pos) & USIC_CH_KSCFG_BPMODEN_Msk);		        //使能USIC（启用外设总线时钟驱动USIC）
	
	Read_KSCFG = USIC0_CH1->KSCFG;          /*手册建议建议在访问其它USIC 寄存器之前先读取 KSCFG，从而避免流水线对控制模块造成的影响*/
	
	WR_REG(USIC0_CH1->FDR, USIC_CH_FDR_DM_Msk, USIC_CH_FDR_DM_Pos, 2);                     //选择分数分频模式
	WR_REG(USIC0_CH1->FDR, USIC_CH_FDR_STEP_Msk, USIC_CH_FDR_STEP_Pos, FDR_STEP);    
	WR_REG(USIC0_CH1->BRG, USIC_CH_BRG_PCTQ_Msk, USIC_CH_BRG_PCTQ_Pos, BRG_PCTQ);
	WR_REG(USIC0_CH1->BRG, USIC_CH_BRG_DCTQ_Msk, USIC_CH_BRG_DCTQ_Pos, BRG_DCTQ);
	WR_REG(USIC0_CH1->BRG, USIC_CH_BRG_PDIV_Msk, USIC_CH_BRG_PDIV_Pos, BRG_PDIV);          //115200 bandrate
	
	WR_REG(USIC0_CH1->DX0CR, USIC_CH_DX0CR_DSEL_Msk, USIC_CH_DX0CR_DSEL_Pos, 0);           //RX:2.2
	WR_REG(USIC0_CH1->INPR, USIC_CH_INPR_RINP_Msk, USIC_CH_INPR_RINP_Pos, 0);              //接收中断服务SR0
    
    WR_REG(USIC0_CH1->SCTR, USIC_CH_SCTR_SDIR_Msk, USIC_CH_SCTR_SDIR_Pos, 0);	           //ASC标准协议，LSB在前
	WR_REG(USIC0_CH1->SCTR, USIC_CH_SCTR_PDL_Msk, USIC_CH_SCTR_PDL_Pos, 1);                //若发送缓存已空，将发送被动数据电平SCTR_PDL
	WR_REG(USIC0_CH1->SCTR, USIC_CH_SCTR_TRM_Msk, USIC_CH_SCTR_TRM_Pos, 1);                //ASC标准 移位控制信号为1视为有效
	WR_REG(USIC0_CH1->SCTR, USIC_CH_SCTR_FLE_Msk, USIC_CH_SCTR_FLE_Pos, 7);                //一个数据帧由多少位数据(7)组成。一个数据帧可由多个数据字串联组成
	WR_REG(USIC0_CH1->SCTR, USIC_CH_SCTR_WLE_Msk, USIC_CH_SCTR_WLE_Pos, 7);                //发送和接收数据字的长度 8，右对齐→→→→→→
	
	WR_REG(USIC0_CH1->TCSR, USIC_CH_TCSR_TDEN_Msk, USIC_CH_TCSR_TDEN_Pos, 1);              //如果TDV=1,则TBUF中的数据字的发送开始（新数据载入发送缓存 TBUF 后， TCSR.TDV 位自动置位）
	WR_REG(USIC0_CH1->TCSR, USIC_CH_TCSR_TDSSM_Msk, USIC_CH_TCSR_TDSSM_Pos, 1);            //数据字TBUF只发送一次
	
	WR_REG(USIC0_CH1->PCR_ASCMode, USIC_CH_PCR_ASCMode_SMD_Msk,	USIC_CH_PCR_ASCMode_SMD_Pos, 1);   //多数表决采样
	WR_REG(USIC0_CH1->PCR_ASCMode, USIC_CH_PCR_ASCMode_STPB_Msk, USIC_CH_PCR_ASCMode_STPB_Pos, 0); //1位停止位
	WR_REG(USIC0_CH1->PCR_ASCMode, USIC_CH_PCR_ASCMode_SP_Msk, USIC_CH_PCR_ASCMode_SP_Pos, 7);     //定义采样点位置（PCR.SP<=BRG.DCTQ)
	WR_REG(USIC0_CH1->PSR_ASCMode, USIC_CH_PSR_TBIF_Msk, USIC_CH_PSR_TBIF_Pos, 1);                 //TBUF中的数据字已载入移位寄存器， TBUF可写入新数据字
	
	WR_REG(USIC0_CH1->CCR, USIC_CH_CCR_MODE_Msk, USIC_CH_CCR_MODE_Pos, 2);				    //ASC协议使用UART
	WR_REG(USIC0_CH1->CCR, USIC_CH_CCR_PM_Msk, USIC_CH_CCR_PM_Pos, 0);					    //无奇偶校验位
	WR_REG(USIC0_CH1->CCR, USIC_CH_CCR_RIEN_Msk, USIC_CH_CCR_RIEN_Pos, 1);				    //使能接收中断
	WR_REG(USIC0_CH1->CCR, USIC_CH_CCR_AIEN_Msk, USIC_CH_CCR_AIEN_Pos, 1);                  //使能备选接收中断
} 

void UART_SendData(uint16_t Send_Data)
{
	SET_BIT(USIC0_CH1->PSCR, USIC_CH_PSCR_CTBIF_Pos);   //清除发送缓存指示标志位
    USIC0_CH1->TBUF[0]    =  Send_Data;                
    while(!(USIC0_CH1->PSR_ASCMode & 0x2000));          //已发生发送缓存事件
} 

void UART_SendBuffer(const uint8_t *Buffer, uint16_t length)
{
	while(length--)
	{
		UART_SendData(*Buffer++);
	}
}
