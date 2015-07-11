/**
  ******************************************************************************
  * @file    	E:\ButterFly\Hardware\JY901\jy901.c
  * @author  	贾一帆
  * @version	V0.0
  * @date  		2015-07-11 09:52:33
  * @brief   	JY-901九轴姿态传感器
  ******************************************************************************
  * @attention
  * 2015-07-11 09:52:54
  * 通过串口读取数据，使用DMA通道
  ******************************************************************************
  */  

/* Includes ------------------------------------------------------------------*/
#include <stm32f10x.h>
#include <string.h>
/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */
#include "sys/sys.h"
#include "delay/delay.h"

#include "JY901/jy901.h"
#include "HC05/hc05.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
JY901Str JY901;
/* Private function prototypes -----------------------------------------------*/
void UARTRxDMARec(JY901Str * JY901);							//开启一次DMA接收
void JY901UartInit(USART_TypeDef * USARTBASE,u32 BaudRate);		//串口初始化
void UARTRxNVICInit(USART_TypeDef * USARTBASE);					//串口的NVIC设置
void JY901DMAInit(JY901Str * JY901);							//DMA初始化
void DMATCNVICInit(DMA_Channel_TypeDef * DMAChannelRx);			//DMA接收通道NVIC配置
/* Private functions ---------------------------------------------------------*/

/**
  *@brief   JY901Init
  *@param   None
  *@retval    None
  */
void JY901Init(JY901Str * JY901){
	
	/* 确认DMA通道号 ------------------------------------------------------------------------------------*/
	if(JY901->USARTBASE == USART1){
		JY901->DMAChannelTx = DMA1_Channel4;	//Tx通道 
		JY901->DMAChannelRx = DMA1_Channel5;	//Rx通道
	}else if(JY901->USARTBASE == USART2){
		JY901->DMAChannelTx = DMA1_Channel7;	//Tx通道 
		JY901->DMAChannelRx = DMA1_Channel6;	//Rx通道
	}else if(JY901->USARTBASE == USART3){
		JY901->DMAChannelTx = DMA1_Channel2;	//Tx通道 
		JY901->DMAChannelRx = DMA1_Channel3;	//Rx通道
	}

	/* 串口初始化 ----------------------------------------------------------------------------------------*/
	JY901UartInit(JY901->USARTBASE,230400);
	
	/* DMA初始化 -----------------------------------------------------------------------------------------*/	
	USART_DMACmd(JY901->USARTBASE,USART_DMAReq_Tx,ENABLE);	//使能串口发送DMA
	USART_DMACmd(JY901->USARTBASE,USART_DMAReq_Rx,ENABLE);	//使能串口接收DMA

	JY901DMAInit(JY901);		//初始化DMA
}


/**
  *@brief   
  *@param   None
  *@retval    None
  */
void USART2_IRQHandler(void){
	static u8 FH[2];		//帧头数组
	static u8 pFH = 0;		//数组下标
	
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET){	//接收到数据
		FH[pFH++] = (unsigned char)JY901.USARTBASE->DR;
		
		if(FH[0] != 0x55){			//若第一个数不是0x55，重新接收
			FH[1] = 0x00;
			pFH = 0;
		}else if(FH[1] == 0x50){
			USART_ITConfig(JY901.USARTBASE,USART_IT_RXNE,DISABLE);	//关闭串口接收中断
			UARTRxDMARec(&JY901);									//开启一次DMA传输
			
			FH[0] = 0x00;		//清空数组
			FH[1] = 0x00;
			pFH = 0;
		}
	}	
}

void DMA1_Channel6_IRQHandler(){
	
	if(DMA_GetITStatus(DMA1_IT_TC6) == SET){
		
		memcpy(&(JY901.Time),&JY901.RxData[1],8);		//时间
		memcpy(&(JY901.Ax),&JY901.RxData[12],6);		//加速度
		memcpy(&(JY901.Wx),&JY901.RxData[23],6);		//角速度	
		memcpy(&(JY901.Ang),&JY901.RxData[34],6);		//角度
		memcpy(&(JY901.Press),&JY901.RxData[45],8);		//气压与高度
		
		/* 显示时间 --------------------------------------------------------------------*/
// 		HC05printf(&HC05,"Time %d-%d-%d %d:%d:%d.%d\r\n",JY901.Time.Year,JY901.Time.Month,JY901.Time.Day,JY901.Time.Hour,JY901.Time.Minute,JY901.Time.Second,JY901.Time.MiliSecond);
		HC05printf(&HC05,"Ang\r\nRow=%f  Pitch=%f  Yaw=%f\r\n",(float)JY901.Ang.Rol/32768*180,(float)JY901.Ang.Pitch/32768*180,(float)JY901.Ang.Yaw/32768*180);
		
		
		USART_ITConfig(JY901.USARTBASE,USART_IT_RXNE,ENABLE);	//打开串口接收中断
		
		JY901.DMAChannelRx->CCR&=~1;      		//关闭DMA传输 

		DMA_ClearITPendingBit(DMA1_IT_GL6);
		
	}
	
}

/**
  *@brief   UARTRxDMARec		启动一次串口的DMA传输
  *@param   JY901Str * JY901		//JY901结构体
  *@retval    None
  */
void UARTRxDMARec(JY901Str * JY901){
	JY901->DMAChannelRx->CCR&=~1;      		//关闭DMA传输 
	JY901->DMAChannelRx->CNDTR=JY901RxLen;  //DMA,传输数据量 
	JY901->DMAChannelRx->CCR|=1;       		//开启DMA传输
}

/**
  *@brief   JY901DMAInit
  *@param   JY901Str * JY901		//JY901结构体
  *@retval    None
  */
void JY901DMAInit(JY901Str * JY901){
	DMA_InitTypeDef DMA_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//使能DMA传输
	delay_us(5);

	/* UARTx  Tx DMA通道初始化 ---------------------------------------------------------------------------------*/
    DMA_DeInit(JY901->DMAChannelTx);   									//将DMA的通道x寄存器重设为缺省值
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(JY901->USARTBASE)->DR);  //DMA外设USART->DR基地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&JY901->TxData;  		//DMA内存基地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST; 			 		//数据传输方向，从内存读取发送到外设
	DMA_InitStructure.DMA_BufferSize = JY901TxLen;  					//DMA通道的DMA缓存的大小
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  	//外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  			//内存地址寄存器递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //数据宽度为8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; 	//数据宽度为8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  						//工作在正常缓存模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh; 			//DMA通道 x拥有中优先级 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  						//DMA通道x没有设置为内存到内存传输
	DMA_Init(JY901->DMAChannelTx, &DMA_InitStructure);  				//根据DMA_InitStruct中指定的参数初始化DMA的通道USARTx_Tx_DMA_Channex所标识的寄存器
	
	
	
	/* UARTx  Rx DMA通道初始化 ---------------------------------------------------------------------------------*/
	DMA_DeInit(JY901->DMAChannelRx);   									//将DMA的通道x寄存器重设为缺省值
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(JY901->USARTBASE)->DR);  //DMA外设USART->DR基地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&JY901->RxData;  		//DMA内存基地址  RxData数组
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; 			 		//数据传输方向，从外设读取并发送到内存
	DMA_InitStructure.DMA_BufferSize = JY901RxLen;  					//DMA通道的DMA缓存的大小
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  	//外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  			//内存地址寄存器递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //数据宽度为8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; 	//数据宽度为8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  						//工作在正常缓存模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_High; 				//DMA通道 x拥有高优先级 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  						//DMA通道x没有设置为内存到内存传输
	DMA_Init(JY901->DMAChannelRx, &DMA_InitStructure);  				//根据DMA_InitStruct中指定的参数初始化DMA的通道USARTx_Rx_DMA_Channex所标识的寄存器

	DMA_ITConfig(JY901->DMAChannelRx, DMA_IT_TC, ENABLE);				//开启传输完成中断
	DMATCNVICInit(JY901->DMAChannelRx);									//配置NVIC			
	
	DMA_SetCurrDataCounter(JY901->DMAChannelTx,0);		//TxDMA通道发送数清空
	DMA_SetCurrDataCounter(JY901->DMAChannelRx,0);		//RxDMA通道发送数清空
}

/**
  *@brief   JY901UartInit	HC-05使用的串口初始化
  *@param   USART_TypeDef * USARTBASE	串口号
  *			u32 BaudRate	波特率
  *@retval    None
  */
void JY901UartInit(USART_TypeDef * USARTBASE,u32 BaudRate){
	u16 GPIO_Pin_Tx;			//Tx引脚
	u16 GPIO_Pin_Rx;			//Rx引脚
	GPIO_TypeDef * GPIOBase;	//GPIOBase
	
	/* 定义初始化结构体 --------------------------------------------------------*/
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	/* 配置参数 ----------------------------------------------------------------*/
	if(USARTBASE == USART1){
		GPIOBase = GPIOA;
		GPIO_Pin_Tx = GPIO_Pin_9;
		GPIO_Pin_Rx = GPIO_Pin_10;
	}else if(USARTBASE == USART2){
		GPIOBase = GPIOA;
		GPIO_Pin_Tx = GPIO_Pin_2;
		GPIO_Pin_Rx = GPIO_Pin_3;
	}else if(USARTBASE == USART3){
		GPIOBase = GPIOB;
		GPIO_Pin_Tx = GPIO_Pin_10;
		GPIO_Pin_Rx = GPIO_Pin_11;
	}

	/* 使能时钟 ----------------------------------------------------------------*/
	if(USARTBASE == USART1){
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);	//使能UART1时钟
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//使能GPIOA时钟
	}else if(USARTBASE == USART2){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);	//使能UART2时钟
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//使能GPIOA时钟
	}else if(USARTBASE == USART3){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);	//使能UART3时钟
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//使能GPIOB时钟
	}

 	USART_DeInit(USARTBASE);  								//复位串口

	/* GPIO端口设置 -------------------------------------------------------------*/
	//USARTx_TX   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_Tx; 			//TXD
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		//复用推挽输出
    GPIO_Init(GPIOBase, &GPIO_InitStructure); 			//初始化TxD

	//USARTx_RX	  
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_Rx;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//浮空输入
    GPIO_Init(GPIOBase, &GPIO_InitStructure);  				//初始化RxD

	/* USARTx 初始化设置 --------------------------------------------------*/
	USART_InitStructure.USART_BaudRate = BaudRate;					//波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//字长为8位数据格式	（默认模式）
	USART_InitStructure.USART_StopBits = USART_StopBits_1;			//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;				//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
    USART_Init(USARTBASE, &USART_InitStructure); 					//初始化串口x
	
	/* 中断配置 -----------------------------------------------------------*/
	USART_ITConfig(USARTBASE,USART_IT_TC,DISABLE);
	USART_ITConfig(USARTBASE,USART_IT_RXNE,ENABLE);					//串口接收中断
    USART_ITConfig(USARTBASE,USART_IT_IDLE,DISABLE);				
	
	
	USART_Cmd(USARTBASE, ENABLE);                  					//使能串口x
	
	/* NVIC 配置 ----------------------------------------------------------*/
	UARTRxNVICInit(USARTBASE);
	
	USART_ClearFlag(USARTBASE, USART_FLAG_TC);						//清除发送成功标志
	
}

/**
  *@brief   RxNVICInit
  *@param   USART_TypeDef * USARTBASE	//串口号
  *@retval    None
  */
void UARTRxNVICInit(USART_TypeDef * USARTBASE){
	IRQn_Type IRQChannel;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* 确认参数 ---------------------------------------------------------------*/
	if(USARTBASE == USART1){
		IRQChannel = USART1_IRQn;
	}else if(USARTBASE == USART2){
		IRQChannel = USART2_IRQn;
	}else if(USARTBASE == USART3){
		IRQChannel = USART3_IRQn;
	}
	
	/* NVIC初始化 -------------------------------------------------------------*/
	NVIC_InitStructure.NVIC_IRQChannel = IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;		//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; 				//子优先级1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					//使能中断号
	NVIC_Init(&NVIC_InitStructure);									//初始化NVIC
}

/**
  *@brief   DMATCNVICInit
  *@param   DMA_Channel_TypeDef * DMAChannelRx	//DMA通道号
  *@retval    None
  */
void DMATCNVICInit(DMA_Channel_TypeDef * DMAChannelRx){
	IRQn_Type IRQChannel;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* 确认参数 ---------------------------------------------------------------*/
	if(DMAChannelRx == DMA1_Channel5){
		IRQChannel = DMA1_Channel5_IRQn;
	}else if(DMAChannelRx == DMA1_Channel6){
		IRQChannel = DMA1_Channel6_IRQn;
	}else if(DMAChannelRx == DMA1_Channel3){
		IRQChannel = DMA1_Channel3_IRQn;
	}
	
	/* NVIC初始化 -------------------------------------------------------------*/
	NVIC_InitStructure.NVIC_IRQChannel = IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;		//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2; 				//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					//使能中断号
	NVIC_Init(&NVIC_InitStructure);									//初始化NVIC
}

/******************* (C) COPYRIGHT 2014 STMicroelectronics *****END OF FILE****/
