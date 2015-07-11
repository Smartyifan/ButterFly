/**
  ******************************************************************************
  * @file    E:\ButterFly\Hardware\JY901\jy901.h
  * @author  ��һ��
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
#define RxDataNum	5			//Ҫ���ܵ�����֡��
#define JY901TxLen  5			//����һ��ָ����ֽ���
#define JY901RxLen  10+11*(RxDataNum-1)	//�������ݳ���
/* TypeDefine ----------------------------------------------------------------*/
typedef struct{
	USART_TypeDef * USARTBASE;

	DMA_Channel_TypeDef * DMAChannelTx;		//DMA����ͨ����
	DMA_Channel_TypeDef * DMAChannelRx;		//DMA����ͨ����

	u8 TxData[JY901TxLen];		//��������
	u8 RxData[JY901RxLen];		//��������

	struct {						//ʱ��	
		unsigned char Year;			//���
		unsigned char Month;		//��
		unsigned char Day;			//��
		unsigned char Hour;			//ʱ
		unsigned char Minute;		//��
		unsigned char Second;		//��
		unsigned short MiliSecond;	//����
	}Time;
	
	struct {			//���ٶ�
		short x;		//����ת����16λ����
		short y;
		short z;
	}Ax;
	
	struct {			//���ٶ�
		short x;		//����ת����16λ����
		short y;
		short z;
	}Wx;
	
	struct {			//�Ƕ�
		short Rol;		//���
		short Pitch;	//����
		short Yaw;		//ƫ��
	}Ang;
	
	struct {			//��ѹ��߶�
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
