
/* 
UART����
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
	WR_REG(SCU_RESET->PRSET0, SCU_RESET_PRSET0_USIC0RS_Msk, SCU_RESET_PRSET0_USIC0RS_Pos, 1);   //��λUSIC���裨�ϵ缴��λ����һ���ɺ��ԣ�
	WR_REG(SCU_RESET->PRCLR0, SCU_RESET_PRCLR0_USIC0RS_Msk, SCU_RESET_PRCLR0_USIC0RS_Pos, 1);   //�����λ
	
	USIC0_CH1->KSCFG =
				((0x1 << USIC_CH_KSCFG_MODEN_Pos) & USIC_CH_KSCFG_MODEN_Msk) |
		        ((0x1 << USIC_CH_KSCFG_BPMODEN_Pos) & USIC_CH_KSCFG_BPMODEN_Msk);		        //ʹ��USIC��������������ʱ������USIC��
	
	Read_KSCFG = USIC0_CH1->KSCFG;          /*�ֲὨ�齨���ڷ�������USIC �Ĵ���֮ǰ�ȶ�ȡ KSCFG���Ӷ�������ˮ�߶Կ���ģ����ɵ�Ӱ��*/
	
	WR_REG(USIC0_CH1->FDR, USIC_CH_FDR_DM_Msk, USIC_CH_FDR_DM_Pos, 2);                     //ѡ�������Ƶģʽ
	WR_REG(USIC0_CH1->FDR, USIC_CH_FDR_STEP_Msk, USIC_CH_FDR_STEP_Pos, FDR_STEP);    
	WR_REG(USIC0_CH1->BRG, USIC_CH_BRG_PCTQ_Msk, USIC_CH_BRG_PCTQ_Pos, BRG_PCTQ);
	WR_REG(USIC0_CH1->BRG, USIC_CH_BRG_DCTQ_Msk, USIC_CH_BRG_DCTQ_Pos, BRG_DCTQ);
	WR_REG(USIC0_CH1->BRG, USIC_CH_BRG_PDIV_Msk, USIC_CH_BRG_PDIV_Pos, BRG_PDIV);          //115200 bandrate
	
	WR_REG(USIC0_CH1->DX0CR, USIC_CH_DX0CR_DSEL_Msk, USIC_CH_DX0CR_DSEL_Pos, 0);           //RX:2.2
	WR_REG(USIC0_CH1->INPR, USIC_CH_INPR_RINP_Msk, USIC_CH_INPR_RINP_Pos, 0);              //�����жϷ���SR0
    
    WR_REG(USIC0_CH1->SCTR, USIC_CH_SCTR_SDIR_Msk, USIC_CH_SCTR_SDIR_Pos, 0);	           //ASC��׼Э�飬LSB��ǰ
	WR_REG(USIC0_CH1->SCTR, USIC_CH_SCTR_PDL_Msk, USIC_CH_SCTR_PDL_Pos, 1);                //�����ͻ����ѿգ������ͱ������ݵ�ƽSCTR_PDL
	WR_REG(USIC0_CH1->SCTR, USIC_CH_SCTR_TRM_Msk, USIC_CH_SCTR_TRM_Pos, 1);                //ASC��׼ ��λ�����ź�Ϊ1��Ϊ��Ч
	WR_REG(USIC0_CH1->SCTR, USIC_CH_SCTR_FLE_Msk, USIC_CH_SCTR_FLE_Pos, 7);                //һ������֡�ɶ���λ����(7)��ɡ�һ������֡���ɶ�������ִ������
	WR_REG(USIC0_CH1->SCTR, USIC_CH_SCTR_WLE_Msk, USIC_CH_SCTR_WLE_Pos, 7);                //���ͺͽ��������ֵĳ��� 8���Ҷ��������������
	
	WR_REG(USIC0_CH1->TCSR, USIC_CH_TCSR_TDEN_Msk, USIC_CH_TCSR_TDEN_Pos, 1);              //���TDV=1,��TBUF�е������ֵķ��Ϳ�ʼ�����������뷢�ͻ��� TBUF �� TCSR.TDV λ�Զ���λ��
	WR_REG(USIC0_CH1->TCSR, USIC_CH_TCSR_TDSSM_Msk, USIC_CH_TCSR_TDSSM_Pos, 1);            //������TBUFֻ����һ��
	
	WR_REG(USIC0_CH1->PCR_ASCMode, USIC_CH_PCR_ASCMode_SMD_Msk,	USIC_CH_PCR_ASCMode_SMD_Pos, 1);   //�����������
	WR_REG(USIC0_CH1->PCR_ASCMode, USIC_CH_PCR_ASCMode_STPB_Msk, USIC_CH_PCR_ASCMode_STPB_Pos, 0); //1λֹͣλ
	WR_REG(USIC0_CH1->PCR_ASCMode, USIC_CH_PCR_ASCMode_SP_Msk, USIC_CH_PCR_ASCMode_SP_Pos, 7);     //���������λ�ã�PCR.SP<=BRG.DCTQ)
	WR_REG(USIC0_CH1->PSR_ASCMode, USIC_CH_PSR_TBIF_Msk, USIC_CH_PSR_TBIF_Pos, 1);                 //TBUF�е���������������λ�Ĵ����� TBUF��д����������
	
	WR_REG(USIC0_CH1->CCR, USIC_CH_CCR_MODE_Msk, USIC_CH_CCR_MODE_Pos, 2);				    //ASCЭ��ʹ��UART
	WR_REG(USIC0_CH1->CCR, USIC_CH_CCR_PM_Msk, USIC_CH_CCR_PM_Pos, 0);					    //����żУ��λ
	WR_REG(USIC0_CH1->CCR, USIC_CH_CCR_RIEN_Msk, USIC_CH_CCR_RIEN_Pos, 1);				    //ʹ�ܽ����ж�
	WR_REG(USIC0_CH1->CCR, USIC_CH_CCR_AIEN_Msk, USIC_CH_CCR_AIEN_Pos, 1);                  //ʹ�ܱ�ѡ�����ж�
} 

void UART_SendData(uint16_t Send_Data)
{
	SET_BIT(USIC0_CH1->PSCR, USIC_CH_PSCR_CTBIF_Pos);   //������ͻ���ָʾ��־λ
    USIC0_CH1->TBUF[0]    =  Send_Data;                
    while(!(USIC0_CH1->PSR_ASCMode & 0x2000));          //�ѷ������ͻ����¼�
} 

void UART_SendBuffer(const uint8_t *Buffer, uint16_t length)
{
	while(length--)
	{
		UART_SendData(*Buffer++);
	}
}
