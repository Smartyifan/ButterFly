/**
  ******************************************************************************
  * @file    
  * @author  贾一帆
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
extern void IIC_Start(void);               //发送开始信号
extern void IIC_Stop(void);                //发送停止信号
extern void IIC_Send_Byte(u8 txd);         //发送一个字节
extern u8 IIC_Read_Byte(unsigned char ack); //读取一个字节
extern u8 IIC_Wait_Ack(void);              //等待发送信号
extern void IIC_Ack(void);                 //发送ACK信号
extern void IIC_NAck(void);                //不发送ACK信号


		 				    
#endif
/******************* (C) COPYRIGHT 2014 STMicroelectronics *****END OF FILE****/
