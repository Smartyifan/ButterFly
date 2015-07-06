/**
  ******************************************************************************
  * @file    
  * @author  贾一帆
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
#define HC05TxDMA	1		//使能TxDMA传输
#define HC05RxDMA	1		//使能RxDMA传输

#define HC05TxLen	50		//HC05Tx DMA 长度
#define HC05RxLen	50		//HC05Rx DMA 长度

#define HC05Key		PCout(7)	//蓝牙控制KEY信号
#define HC05LED		PCin(8)		//蓝牙连接状态信号
/* TypeDefine ----------------------------------------------------------------*/
typedef struct HC05{
	USART_TypeDef * USARTBASE;
	
	u32 KeyBase;
	u32 LEDBase;	
	u16 KeyPin;
	u16 LEDPin;
	
	ErrorStatus TxSuccess;		//DMA传输完成标志
	ErrorStatus Checked;		//检查标志
	
	u16 RxLen;					//接收到的数据长度
	
	
	u8 TxData[HC05TxLen];		//发送数组
	u8 RxData[HC05RxLen];		//接收数据
}HC05Str;
/* extern Variables ----------------------------------------------------------*/
extern HC05Str HC05;		//HC05结构体	
/* extern function------------------------------------------------------------*/
extern ErrorStatus HC05Init(HC05Str * HC05);						//初始化HC05
extern void UARTxDMASend(DMA_Channel_TypeDef*DMA_CHx,u16 Len);		//通过DMA发送一定长度的数组
extern void HC05printf(HC05Str * HC05,DMA_Channel_TypeDef*DMA_CHx,char* fmt,...);	//基于DMA传输的printf
#endif
/******************* (C) COPYRIGHT 2014 STMicroelectronics *****END OF FILE****/
