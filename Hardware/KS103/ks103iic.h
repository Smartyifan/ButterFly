/**
  ******************************************************************************
  * @file    
  * @author  ��һ��
  * @version V3.5.0
  * @date    2014-10-4
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */  
  
#ifndef _KS103IIC_H
#define _KS103IIC_H
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

#include "KS103/ks103.h"
/* Define --------------------------------------------------------------------*/
#define SDA_IN() {GPIOF->CRL&=0xfffffff0;GPIOF->CRL|=0x00000008;}	/*GPIOB->ODR|=1<<11;*/
#define SDA_OUT() {GPIOF->CRL&=0xfffffff0;GPIOF->CRL|=0x00000003;}

#define SCL PFout(1)
#define SDA PFout(0)
#define READ_SDA PFin(0)

/*extern function-------------------------------------------------------------*/
extern void I2CInit(KS103Str * KS103);
extern void IIC_Start(void);               //���Ϳ�ʼ�ź�
extern void IIC_Stop(void);                //����ֹͣ�ź�
extern void IIC_Send_Byte(u8 txd);         //����һ���ֽ�
extern u8 IIC_Read_Byte(unsigned char ack); //��ȡһ���ֽ�
extern u8 IIC_Wait_Ack(void);              //�ȴ������ź�
extern void IIC_Ack(void);                 //����ACK�ź�
extern void IIC_NAck(void);                //������ACK�ź�


		 				    
#endif
/******************* (C) COPYRIGHT 2014 STMicroelectronics *****END OF FILE****/
