/**
  ******************************************************************************
  * @file    E:\ButterFly\Hardware\JY901\jy901.h
  * @author  贾一帆
  * @version V0.0
  * @date    2015-07-11 09:54:46
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */  
  
#ifndef  _JY901_H
#define  _JY901_H
/* Includes ------------------------------------------------------------------*/
#include <stm32f10x.h>
#include "sys/sys.h"

/* Define --------------------------------------------------------------------*/
#define RxDataNum	5			//要接受的数据帧数
#define JY901TxLen  5			//发送一个指令的字节数
#define JY901RxLen  10+11*(RxDataNum-1)	//接收数据长度
/* TypeDefine ----------------------------------------------------------------*/
typedef struct{
	USART_TypeDef * USARTBASE;

	DMA_Channel_TypeDef * DMAChannelTx;		//DMA接收通道号
	DMA_Channel_TypeDef * DMAChannelRx;		//DMA发送通道号

	u8 TxData[JY901TxLen];		//发送数组
	u8 RxData[JY901RxLen];		//接收数据

	struct {						//时间	
		unsigned char Year;			//年份
		unsigned char Month;		//月
		unsigned char Day;			//日
		unsigned char Hour;			//时
		unsigned char Minute;		//分
		unsigned char Second;		//秒
		unsigned short MiliSecond;	//毫秒
	}Time;
	
	struct {			//加速度
		short x;		//不做转换，16位数据
		short y;
		short z;
	}Ax;
	
	struct {			//角速度
		short x;		//不做转换，16位数据
		short y;
		short z;
	}Wx;
	
	struct {			//角度
		short Rol;		//横滚
		short Pitch;	//俯仰
		short Yaw;		//偏航
	}Ang;
	
	struct {			//气压与高度
		long lPressure;
		long lAltitude;
	}Press;
} JY901Str;
/* extern Variables ----------------------------------------------------------*/
extern JY901Str JY901;
/* extern function------------------------------------------------------------*/
extern void JY901Init(JY901Str * JY901);
		 				    
#endif
/******************* (C) COPYRIGHT 2014 STMicroelectronics *****END OF FILE****/
