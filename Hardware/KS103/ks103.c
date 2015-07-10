/**
  ******************************************************************************
  * @file    E:\ButterFly\Hardware\KS103\ks103.c
  * @author  贾一帆
  * @version V0.0
  * @date    2015-07-06 21:18:24
  * @brief   Blank
  ******************************************************************************
  * @attention
  * 2015-07-09 22:06:05
  * 探测过程：
  * 1.使用Read_KS10X_Data函数执行探测命令，打开外部中断。range为量程与精度的指令
  * 2.执行完上述函数后，需要等待几十ms至探测完成传回数据，为了减少程序的占用率，
  *   使用外部中断。SCL线在探测完成前为低电平，探测完成后为高电平。在中断程序中
  *   置位探测完成标志并读取数据，最后屏蔽中断，数据存放在结构体中。
  *   外部中断优先级为最高级0级
  * 2015-07-10 10:17:17
  * 使用方法：
  * 1.在ks103iic.h中设置引脚相关的宏定义
  * 2.在初始化中设置SDA、SCL的引脚
  * 3.编写进入外部中断(SCLInEXTI)、外部中断服务函数(EXTIx_IRQHandler)
  * 4.每次探测时，先调用Read_KS10X_Data，之后读取KS103.detected标志位，若为SUCCESS
  *   则可以读取KS103.Height数据
  ******************************************************************************
  */  
  
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */
#include "sys/sys.h"
#include "delay/delay.h"
  
#include "KS103/ks103.h"
#include "KS103/ks103iic.h"

#include "HC05/hc05.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
KS103Str KS103;
/* Private function prototypes -----------------------------------------------*/
void SCLEXTIInit(KS103Str * KS103);										//SCL线的外部中断初始化，在读取数据时用到
void SCLInEXTI(KS103Str * KS103);										//进入外部中断模式			
void Read_KS10X_Data(KS103Str * KS103, u8 range);						//根据量程指令读取高度	
void Single_WriteI2C(u8 SlaveAddress,u8 REG_Address,u8 REG_data);		//利用I2C读一个字节
u8 Single_ReadI2C(u8 SlaveAddress,u8 REG_Address);						//利用I2C写一个字节
void Read_KS10X(KS103Str * KS103);										//读16位数据
void Change_Addr(u8 OldAdddr,u8 NewAddr);								//设置新地址
/* Private functions ---------------------------------------------------------*/
/**
  *@brief   Function Name	
  *@param   None;
  *@retval  None
  */
ErrorStatus KS103Init(KS103Str * KS103){
	u8 Year,Week;
	
	I2CInit(KS103);		//I2C初始化
	SCLEXTIInit(KS103);	//外部中断初始化
	
	KS103->Checked = ERROR;
	
	Year = Single_ReadI2C(KS103->SlaveAddress,0x00);			//读取生产时间信息
// 	HC05printf(&HC05,DMA1_Channel4,"Year = %x\r\n",Year);
	Week = Single_ReadI2C(KS103->SlaveAddress,0x01);
// 	HC05printf(&HC05,DMA1_Channel4,"Week = %x\r\n",Week);
	if(Week == 0x1f && Year == 0x0e)KS103->Checked = SUCCESS;			
	
// 	Single_WriteI2C(KS103->SlaveAddress,2,0x71);		//第二级降噪,适用于USB供电，只执行一次，断电保存配置
// 	delay_ms(2000);
	
	return KS103->Checked;
}
 

/**
  *@brief   SCLInEXTI		//SCL线进入外部中断
  *@param   None
  *@retval    None
  */
void SCLInEXTI(KS103Str * KS103){
	GPIOF->CRL&=0xffffff0f;
	GPIOF->CRL|=0x00000080;		
	GPIOF->BRR = 0x0002;		//配置为GPIOF.1下拉输入

	EXTI->IMR|=0x0002;			//不屏蔽1线中断
}

/**
  *@brief   EXTI1_IRQHandler		//外部中断服务函数
  *@param   None
  *@retval    None
  */
void EXTI1_IRQHandler(){
	
	EXTI_ClearITPendingBit(EXTI_Line1);    	//清除1线中断标志
		
	EXTI->IMR&=~0x0002;					//屏蔽1线上的外部中断

	GPIOF->CRL &= 0xffffff0f;
	GPIOF->CRL |= 0x00000030;				//SCL引脚推挽输出

	Read_KS10X(&KS103); 					//读取数据
		
	KS103.detected = SUCCESS;				//探测成功
}

/**
  *@brief   Read_KS10X_Data		//根据量程指令读取高度	
  *@param   None
  *@retval    None
  */
void Read_KS10X_Data(KS103Str * KS103,u8 range){	//根据量程指令读取高度		
	
	KS103->detected = ERROR;						//未探测成功
	Single_WriteI2C(KS103->SlaveAddress,2,range);	//高精度5m量程探测,最长测量时间68ms
	
	SCLInEXTI(KS103);								//进入等待外部中断状态，外部中断中将探测成功标志置位
}
void Single_WriteI2C(u8 SlaveAddress,u8 REG_Address,u8 REG_data){	//I2C写一个字节      
    IIC_Start();                    //起始信号
    IIC_Send_Byte(SlaveAddress);   	//发送设备地址+写信号
 
    while(IIC_Wait_Ack());
 
    IIC_Send_Byte(REG_Address);    //内部寄存器地址
 
    while(IIC_Wait_Ack());                         
                                    
    IIC_Send_Byte(REG_data);       //内部寄存器数据
 
    while(IIC_Wait_Ack());                                 
    IIC_Stop();                     //发送停止信号
}         

u8 Single_ReadI2C(u8 SlaveAddress,u8 REG_Address){		//利用I2C读一个字节
    u8 REG_data=0;
    IIC_Start();   
    IIC_Send_Byte(SlaveAddress);   
 
    while(IIC_Wait_Ack());
    IIC_Send_Byte(REG_Address);
 
    while(IIC_Wait_Ack());                             
    IIC_Start();
    IIC_Send_Byte(SlaveAddress+1);  
     
    while(IIC_Wait_Ack());
 
    delay_us(50);
 
    REG_data=IIC_Read_Byte(0);
	IIC_Stop(); 
	return REG_data;
}

void Read_KS10X(KS103Str * KS103){										//读16位数据
	KS103->Height=Single_ReadI2C(KS103->SlaveAddress,0x02);             //高8位
	KS103->Height<<=8;
	KS103->Height+=Single_ReadI2C(KS103->SlaveAddress,0x03);                //低8位
}


void Change_Addr(u8 OldAdddr,u8 NewAddr){			//设置新地址
	Single_WriteI2C(OldAdddr,0x02,0x9a);            //原地址
	delay_ms(1);
	Single_WriteI2C(OldAdddr,0x02,0x92);
	delay_ms(1);
	Single_WriteI2C(OldAdddr,0x02,0x9e);
	delay_ms(1);
	Single_WriteI2C(OldAdddr,0x02,NewAddr);
	delay_ms(100);
}
 


/**
  *@brief   SDAEXTIInit		SDA线等待数据时的外部中断
  *@param   None
  *@retval    None
  */
void SCLEXTIInit(KS103Str * KS103){
	/* 初始化结构体定义 ------------------------------------------------*/
    EXTI_InitTypeDef EXTI_InitStructure;    
    NVIC_InitTypeDef NVIC_InitStructure;
	
	/* 启动时钟 --------------------------------------------------------*/
	//GPIO引脚在GPIOInit中已使能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE); //使能时钟复用引脚
	

	/* 外部中断初始化 --------------------------------------------------*/
	GPIO_EXTILineConfig(KS103->I2C.ClkPortSrc,KS103->I2C.ClkPinSrc);   	
	EXTI_InitStructure.EXTI_Line = KS103->I2C.ClkPin;                     	
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;             //中断事件
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;         	//上升沿触发
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;                       //中断线使能
    EXTI_Init(&EXTI_InitStructure);                                 //初始化中断
	
	NVIC_InitStructure.NVIC_IRQChannel = KS103->I2C.ClkPinSrc+6;      //中断号IRQn
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;       //抢占优先级0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;       //子优先级0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);                                 //中断管理器初始化
	
	EXTI->IMR&=~(1<<KS103->I2C.ClkPinSrc);							//屏蔽线上的外部中断
}

/******************* (C) COPYRIGHT 2014 STMicroelectronics *****END OF FILE****/
