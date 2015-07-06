/**
  ******************************************************************************
  * @file    
  * @author  ��һ��
  * @version 
  * @date    
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */  
  
#ifndef _HC05_H
#define _HC05_H
/* Includes ------------------------------------------------------------------*/
#include <stm32f10x.h>
#include "sys/sys.h"

/* Define --------------------------------------------------------------------*/
#define HC05TxDMA	1		//ʹ��TxDMA����
#define HC05RxDMA	1		//ʹ��RxDMA����

#define HC05TxLen	50		//HC05Tx DMA ����
#define HC05RxLen	50		//HC05Rx DMA ����

#define HC05Key		PCout(7)	//��������KEY�ź�
#define HC05LED		PCin(8)		//��������״̬�ź�
/* TypeDefine ----------------------------------------------------------------*/
typedef struct HC05{
	USART_TypeDef * USARTBASE;
	
	u32 KeyBase;
	u32 LEDBase;	
	u16 KeyPin;
	u16 LEDPin;
	
	ErrorStatus TxSuccess;		//DMA������ɱ�־
	ErrorStatus Checked;		//����־
	
	u16 RxLen;					//���յ������ݳ���
	
	
	u8 TxData[HC05TxLen];		//��������
	u8 RxData[HC05RxLen];		//��������
}HC05Str;
/* extern Variables ----------------------------------------------------------*/
extern HC05Str HC05;		//HC05�ṹ��	
/* extern function------------------------------------------------------------*/
extern ErrorStatus HC05Init(HC05Str * HC05);						//��ʼ��HC05
extern void UARTxDMASend(DMA_Channel_TypeDef*DMA_CHx,u16 Len);		//ͨ��DMA����һ�����ȵ�����
extern void HC05printf(HC05Str * HC05,DMA_Channel_TypeDef*DMA_CHx,char* fmt,...);	//����DMA�����printf
#endif
/******************* (C) COPYRIGHT 2014 STMicroelectronics *****END OF FILE****/
