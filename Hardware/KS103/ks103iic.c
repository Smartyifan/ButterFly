/**
  ******************************************************************************
  * @file    路径
  * @author  贾一帆
  * @version V3.5.0
  * @date    Date
  * @brief   Blank
  ******************************************************************************
  * @attention
  *
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
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void IIC_Start(void);               //发送开始信号
void IIC_Stop(void);                //发送停止信号
void IIC_Send_Byte(u8 txd);         //发送一个字节
u8 IIC_Read_Byte(unsigned char ack); //读取一个字节
u8 IIC_Wait_Ack(void);              //等待发送信号
void IIC_Ack(void);                 //发送ACK信号
void IIC_NAck(void);                //不发送ACK信号
//void Change_Addr(u8 NewAddr);
/* Private functions ---------------------------------------------------------*/
/**
  *@brief   I2CInit	
  *@param   KS103Str * KS103	指向KS103Str结构体的指针
  *@retval  None
  */
void I2CInit(KS103Str * KS103){
	GPIO_InitTypeDef GPIO_InitStructure;
	/* 启动时钟 ------------------------------------------------------------------------------*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA<<((KS103->I2C.ClkGPIOBase-0x40010800)/0x00000400),ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA<<((KS103->I2C.SDAGPIOBase-0x40010800)/0x00000400),ENABLE);
	
	
	/* 设置中断线 ----------------------------------------------------------------------------*/
	KS103->I2C.ClkPortSrc=(u8)((KS103->I2C.ClkGPIOBase-0x40010800)/0x00000400);
	
	switch (KS103->I2C.ClkPin){
		case GPIO_Pin_0:{KS103->I2C.ClkPinSrc = GPIO_PinSource0;
			break;}
		case GPIO_Pin_1:{KS103->I2C.ClkPinSrc = GPIO_PinSource1;
			break;}
		case GPIO_Pin_2:{KS103->I2C.ClkPinSrc = GPIO_PinSource2;
			break;}
		case GPIO_Pin_3:{KS103->I2C.ClkPinSrc = GPIO_PinSource3;
			break;}
		case GPIO_Pin_4:{KS103->I2C.ClkPinSrc = GPIO_PinSource4;
			break;}
		case GPIO_Pin_5:{KS103->I2C.ClkPinSrc = GPIO_PinSource5;
			break;}
		case GPIO_Pin_6:{KS103->I2C.ClkPinSrc = GPIO_PinSource6;
			break;}
		case GPIO_Pin_7:{KS103->I2C.ClkPinSrc = GPIO_PinSource7;
			break;}
		case GPIO_Pin_8:{KS103->I2C.ClkPinSrc = GPIO_PinSource8;
			break;}
		case GPIO_Pin_9:{KS103->I2C.ClkPinSrc = GPIO_PinSource9;
			break;}
		case GPIO_Pin_10:{KS103->I2C.ClkPinSrc = GPIO_PinSource10;
			break;}
		case GPIO_Pin_11:{KS103->I2C.ClkPinSrc = GPIO_PinSource11;
			break;}
		case GPIO_Pin_12:{KS103->I2C.ClkPinSrc = GPIO_PinSource12;
			break;}
		case GPIO_Pin_13:{KS103->I2C.ClkPinSrc = GPIO_PinSource13;
			break;}
		case GPIO_Pin_14:{KS103->I2C.ClkPinSrc = GPIO_PinSource14;
			break;}
		case GPIO_Pin_15:{KS103->I2C.ClkPinSrc = GPIO_PinSource15;
			break;}
		default:
			break;
	}

	/* I2C ClkPin Init ------------------------------------------------------------*/
	GPIO_InitStructure.GPIO_Pin= KS103->I2C.ClkPin;			 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//50MHz输出频率
	GPIO_Init((GPIO_TypeDef *)KS103->I2C.ClkGPIOBase,&GPIO_InitStructure);	//初始化
	GPIO_SetBits((GPIO_TypeDef *)KS103->I2C.ClkGPIOBase,KS103->I2C.ClkPin);
	
	/* I2C SDAPin Init ------------------------------------------------------------*/
	GPIO_InitStructure.GPIO_Pin= KS103->I2C.SDAPin;			 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//50MHz输出频率
	GPIO_Init((GPIO_TypeDef *)KS103->I2C.SDAGPIOBase,&GPIO_InitStructure);	//初始化
	GPIO_SetBits((GPIO_TypeDef *)KS103->I2C.SDAGPIOBase,KS103->I2C.SDAPin);	//SDA引脚拉高
}
 
/**
  *@brief   IIC_Start
  *@param   None
  *@retval    None
  */
void IIC_Start(void){
	SDA_OUT();		
	SDA = 1;
	SCL = 1;
	delay_us(10);
	
	SDA = 0;
	delay_us(10);
	
	SCL = 0;
}

/**
  *@brief   IIC_Stop
  *@param   None
  *@retval    None
  */
void IIC_Stop(void){
    SDA_OUT();                  //sda输出
    SCL=0;
    SDA=0;
    delay_us(10);
    SCL=1;                      //STOP:when CLK is high DATA change form low to high
    SDA=1;                      //发送I2C总线结束信号
    delay_us(10);                          
}

/**
  *@brief   IIC_Wait_Ack
  *@param   None
  *@retval    None
  */
u8 IIC_Wait_Ack(void){
    u8 ucErrTime=0;
    SDA_IN();     	 		//SDA输入
    SDA=1;delay_us(6);    
    SCL=1;delay_us(6);  
     
    while(READ_SDA)
    {
        ucErrTime++;
        if(ucErrTime>250)
        {
            IIC_Stop();
            return 1;
        }
    }
    SCL=0;                             //时钟输出0 
    return 0; 
}

/**
  *@brief   IIC_Ack	发送ACK应答
  *@param   None
  *@retval    None
  */
void IIC_Ack(void){
    SCL=0;
    SDA_OUT();
    SDA=0;
    delay_us(10);
    SCL=1;
    delay_us(10);
    SCL=0;
}

/**
  *@brief   IIC_NAck	不发送ACK应答	
  *@param   None
  *@retval    None
  */
void IIC_NAck(void){
    SCL=0;
    SDA_OUT();
    SDA=1;
    delay_us(10);
    SCL=1;
    delay_us(10);
    SCL=0;

}

void IIC_Send_Byte(u8 txd)
{                       
    u8 t;  
    SDA_OUT();     
    SCL=0;                                    //拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {             
        SDA=(txd&0x80)>>7;
        txd<<=1;   
        delay_us(10);                          //对TEA5767这三个延时都是必须的
        SCL=1;
        delay_us(10);
        SCL=0; 
        delay_us(10);
    }   
}      

u8 IIC_Read_Byte(unsigned char ack)
{
    unsigned char i,receive=0;
    SDA_IN();                                //SDA?????
 
    for(i=0;i<8;i++ )
    {
        SCL=0;
        delay_us(10);
        SCL=1;
        receive<<=1;
        if(READ_SDA)receive++;  
        delay_us(5);
    }                   
     
    if (!ack)
        IIC_NAck();                         //??nACK
    else
        IIC_Ack();                          //??ACK 
     
    return receive;
}

/******************* (C) COPYRIGHT 2014 STMicroelectronics *****END OF FILE****/
