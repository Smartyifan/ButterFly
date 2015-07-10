/**
  ******************************************************************************
  * @file    E:\ButterFly\Hardware\KS103\ks103.h
  * @author  刘俊
  * @version V0.0
  * @date    2015-07-06 21:20:06
  * @brief   KS103超声波模块
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */  
  
#ifndef  _KS103_H
#define  _KS103_H
/* Includes ------------------------------------------------------------------*/
#include <stm32f10x.h>

/* Define --------------------------------------------------------------------*/
#define SlaveAddress1   0xE8    //IIC写入时的地址字节数据，+1为读
#define SlaveAddress2   0xd0
#define SlaveAddress3   0xd2
#define SlaveAddress4   0xd4
#define SlaveAddress5   0xd6
#define SlaveAddress6   0xd8
#define SlaveAddress7   0xda
#define SlaveAddress8   0xdc
#define SlaveAddress9   0xde
#define SlaveAddress10  0xe0
#define SlaveAddress11  0xe2
#define SlaveAddress12  0xe4
#define SlaveAddress13  0xe6
#define SlaveAddress14  0xea
#define SlaveAddress15  0xec
#define SlaveAddress16  0xee
#define SlaveAddress17  0xf8
#define SlaveAddress18  0xfa
#define SlaveAddress19  0xfc
#define SlaveAddress20  0xfe

/* TypeDefine ----------------------------------------------------------------*/
typedef struct KS103{
	struct {
		u32 ClkGPIOBase;
		u32 SDAGPIOBase;
		u16 ClkPin;
		u16 SDAPin;
		u8 ClkPortSrc;		//外部中断号
		u16 ClkPinSrc;		//外部中断线
	}I2C;
	
	ErrorStatus Checked;	//检查到KS103模块标志
	ErrorStatus detected;	//探测成功标志	
	
	u8 SlaveAddress;		//从设备地址
	u16 Height;				//读出的高度
}KS103Str;
/* extern Variables ----------------------------------------------------------*/
extern KS103Str KS103;

/* extern function------------------------------------------------------------*/
extern ErrorStatus KS103Init(KS103Str * KS103);									//KS103初始化	
extern void Change_Addr(u8 OldAdddr,u8 NewAddr);								//设置新地址
extern void Read_KS10X_Data(KS103Str * KS103,u8 range);							//根据量程指令读取高度
		 				    
#endif
/******************* (C) COPYRIGHT 2014 STMicroelectronics *****END OF FILE****/
