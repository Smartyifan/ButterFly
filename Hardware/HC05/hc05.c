/**
  ******************************************************************************
  * @file    	E:\ButterFly\Hardware\HC05\hc05.c
  * @author  	贾一帆（借鉴正点原子）
  * @version	V0.0
  * @date  		2015-07-05 15:59:29
  * @brief   	HC-05蓝牙串口模块
  ******************************************************************************
  * @attention
  *	2015-07-06 00:08:45
  * 只能使用USART1~3
  * 串口接收中断的抢占优先级为1，子优先级为0，可以在RxNVICInit函数中修改
  * 在头文件中设置HC05TxDMA和HC05RxDMA的值决定是否用到DMA传输（默认使用DMA）
  * 在头文件中设置DMA发送和接收内存的长度
  ******************************************************************************
  */  

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stdio.h"
#include "string.h"
#include "stdarg.h"	 
/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */
#include "sys/sys.h"  
#include "delay/delay.h"

#include "HC05/hc05.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
HC05Str HC05;		//HC05结构体	
/* Private function prototypes -----------------------------------------------*/
void HC05printf(HC05Str * HC05,DMA_Channel_TypeDef*DMA_CHx,char* fmt,...);	//基于DMA传输的printf
void RxDataHandler(u8 * Data,u16 Len);						//接收数据处理函数
void UARTxDMASend(DMA_Channel_TypeDef*DMA_CHx,u16 Len);		//启动一次串口的DMA传输
void UartInit(USART_TypeDef * USARTBASE,u32 BaudRate);		//串口初始化
void DMAInit(HC05Str * HC05);								//DMA初始化
void RxNVICInit(USART_TypeDef * USARTBASE);					//NVIC初始化
void GPIOInit(HC05Str * HC05);								//GPIO初始化
/* Private functions ---------------------------------------------------------*/
/**
  *@brief   HC05Init
  *@param   None;
  *@retval  None
  */
ErrorStatus HC05Init(HC05Str * HC05){
	u8 timeout = 10,wait;		//检查超时
	DMA_Channel_TypeDef * DMAChannelTx;
	
	HC05->Checked = ERROR;		//检查HC05模块成功标志
	/* 确认参数 ---------------------------------------------------------------------------------------------*/
	if(HC05->USARTBASE == USART1){
		DMAChannelTx = DMA1_Channel4;	//Tx通道 
	}else if(HC05->USARTBASE == USART2){
		DMAChannelTx = DMA1_Channel7;	//Tx通道 
	}else if(HC05->USARTBASE == USART3){
		DMAChannelTx = DMA1_Channel2;	//Tx通道 
	}

	
	GPIOInit(HC05);
	UartInit(HC05->USARTBASE,9600);		//初始化中断
	
	/* DMA初始化 ---------------------------------------------------*/
	#if HC05TxDMA	//若使能TxDMA传输	
	USART_DMACmd(HC05->USARTBASE,USART_DMAReq_Tx,ENABLE);
	#endif
	#if HC05RxDMA	//若使能RxDMA传输	
	USART_DMACmd(HC05->USARTBASE,USART_DMAReq_Rx,ENABLE);
	#endif
	
	DMAInit(HC05);
	DMA_SetCurrDataCounter(DMAChannelTx,0);		//TxDMA通道发送数清空，使之能执行第一次HC05Pintf函数
	
	while(timeout--){
		HC05Key = 1;			//KEY=1,进入AT模式
		delay_ms(10);
		
		HC05printf(HC05,DMAChannelTx,"AT\r\n");
		HC05Key = 0;			//KEY = 0,退出AT模式
		
		for(wait = 0;wait < 10;wait ++){			//等待50ms接收HC05模块的回应
			if(HC05->Checked == SUCCESS)return HC05->Checked;;	//若接收成功，退出Init函数	
			delay_ms(10);					
		}
	}
	
	
	return ERROR;		//返回检测标志
}

/**
  *@brief   USART1_IRQHandler	USART1中断服务函数
  *@param   None
  *@retval  None
  *@brief   进入中断后关闭DMA传输，如需再次传输，需要在数据处理完成后再次打开
  */
void USART1_IRQHandler(){
	u32 reg = 0;
	u8 i;
	HC05.RxLen = 0;
	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET) {	//进入空闲中断
		DMA_Cmd(DMA1_Channel5,DISABLE); 					//关闭串口1的接收DMA通道
		reg = USART1->SR;
		reg = USART1->DR;		//这两条语句用于清除IDLE中断标志
		
		HC05.RxLen = HC05RxLen - DMA_GetCurrDataCounter(DMA1_Channel5);			//计算接收到的长度
		
		if(HC05.RxLen == 4 && HC05.RxData[0] == 'O' && HC05.RxData[1] == 'K')	//接收到'OK\r\n'
			HC05.Checked = SUCCESS;												//检测到HC-05	
		
		DMA_SetCurrDataCounter(DMA1_Channel5,HC05RxLen);		//再次设置接收长度
		DMA_Cmd(DMA1_Channel5,ENABLE); 							//使能串口5的接收DMA通道
	} 
}


/**
  *@brief   
  *@param   None
  *@retval    None
  */
void HC05printf(HC05Str * HC05,DMA_Channel_TypeDef*DMA_CHx,char* fmt,...){
	va_list ap;
	va_start(ap,fmt);
	
	vsprintf((char*)HC05->TxData,fmt,ap);
	va_end(ap);
	while(DMA_CHx->CNDTR!=0);	//等待通道7传输完成   
	UARTxDMASend(DMA_CHx,strlen((const char*)HC05->TxData)); 	//通过dma发送出去
}


/**
  *@brief   UARTxDMASend		启动一次串口的DMA传输
  *@param   DMA_Channel_TypeDef*DMA_CHx		//DMA通道
  * 		u16 Len							//数据长度
  *@retval    None
  */
void UARTxDMASend(DMA_Channel_TypeDef*DMA_CHx,u16 Len){
	DMA_CHx->CCR&=~(1<<0);      //关闭DMA传输 
	DMA_CHx->CNDTR=Len;    		//DMA,传输数据量 
	DMA_CHx->CCR|=1<<0;         //开启DMA传输
}



/**
  *@brief   GPIOInit	GPIO初始化
  *@param   None
  *@retval    None
  */
void GPIOInit(HC05Str * HC05){
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* 启动时钟 ------------------------------------------------------------------------------*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA<<((HC05->KeyBase-0x40010800)/0x00000400),ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA<<((HC05->LEDBase-0x40010800)/0x00000400),ENABLE);
	
	/* Key Pin Init ------------------------------------------------------------*/
	GPIO_InitStructure.GPIO_Pin= HC05->KeyPin; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//50MHz输出频率
	GPIO_Init((GPIO_TypeDef *)HC05->KeyBase,&GPIO_InitStructure);	//初始化
	
	/* LED Pin Init ------------------------------------------------------------*/
	GPIO_InitStructure.GPIO_Pin= HC05->LEDPin; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;					//上拉输入
	GPIO_Init((GPIO_TypeDef *)HC05->LEDBase,&GPIO_InitStructure);	//初始化
}
/**
  *@brief   
  *@param   None
  *@retval    None
  */
void DMAInit(HC05Str * HC05){
	DMA_Channel_TypeDef * DMAChannelTx;
	DMA_Channel_TypeDef * DMAChannelRx;
	
	DMA_InitTypeDef DMA_InitStructure;
	
	/* 确认参数 ---------------------------------------------------------------------------------------------*/
	if(HC05->USARTBASE == USART1){
		DMAChannelTx = DMA1_Channel4;	//Tx通道 
		DMAChannelRx = DMA1_Channel5;	//Rx通道
	}else if(HC05->USARTBASE == USART2){
		DMAChannelTx = DMA1_Channel7;	//Tx通道 
		DMAChannelRx = DMA1_Channel6;	//Rx通道
	}else if(HC05->USARTBASE == USART3){
		DMAChannelTx = DMA1_Channel2;	//Tx通道 
		DMAChannelRx = DMA1_Channel3;	//Rx通道
	}

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//使能DMA传输
	delay_us(5);

	#if HC05TxDMA								//若使能TxDMA传输	
	/* UARTx  Tx DMA通道初始化 ---------------------------------------------------------------------------------*/
    DMA_DeInit(DMAChannelTx);   										//将DMA的通道x寄存器重设为缺省值
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(HC05->USARTBASE)->DR);  //DMA外设USART->DR基地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&HC05->TxData;  		//DMA内存基地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST; 			 		//数据传输方向，从内存读取发送到外设
	DMA_InitStructure.DMA_BufferSize = HC05TxLen;  						//DMA通道的DMA缓存的大小
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  	//外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  			//内存地址寄存器递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //数据宽度为8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; 	//数据宽度为8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  						//工作在正常缓存模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; 				//DMA通道 x拥有中优先级 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  						//DMA通道x没有设置为内存到内存传输
	DMA_Init(DMAChannelTx, &DMA_InitStructure);  						//根据DMA_InitStruct中指定的参数初始化DMA的通道USARTx_Tx_DMA_Channex所标识的寄存器
	
	
	#endif
	
	#if HC05RxDMA							//若使能RxDMA传输	
	/* UARTx  Rx DMA通道初始化 ---------------------------------------------------------------------------------*/
	DMA_DeInit(DMAChannelRx);   										//将DMA的通道x寄存器重设为缺省值
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(HC05->USARTBASE)->DR);  //DMA外设USART->DR基地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&HC05->RxData;  		//DMA内存基地址  RxData数组
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; 			 		//数据传输方向，从外设读取并发送到内存
	DMA_InitStructure.DMA_BufferSize = HC05RxLen;  							//DMA通道的DMA缓存的大小
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  	//外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  			//内存地址寄存器递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //数据宽度为8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; 	//数据宽度为8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  						//工作在正常缓存模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh; 			//DMA通道 x拥有超高优先级 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  						//DMA通道x没有设置为内存到内存传输
	DMA_Init(DMAChannelRx, &DMA_InitStructure);  						//根据DMA_InitStruct中指定的参数初始化DMA的通道USARTx_Rx_DMA_Channex所标识的寄存器
	#endif
}

#if HC05RxDMA
/**
  *@brief   RxNVICInit
  *@param   USART_TypeDef * USARTBASE	//串口号
  *@retval    None
  */
void RxNVICInit(USART_TypeDef * USARTBASE){
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
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;		//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 				//子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					//使能中断号
	NVIC_Init(&NVIC_InitStructure);									//初始化NVIC
}
#endif


/**
  *@brief   UartInit	HC-05使用的串口初始化
  *@param   USART_TypeDef * USARTBASE	串口号
  *			u32 BaudRate	波特率
  *@retval    None
  */
void UartInit(USART_TypeDef * USARTBASE,u32 BaudRate){
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
	USART_ITConfig(USARTBASE,USART_IT_RXNE,DISABLE);
    USART_ITConfig(USARTBASE,USART_IT_IDLE,ENABLE);					//串口空闲中断
	
	
	USART_Cmd(USARTBASE, ENABLE);                  					//使能串口x
	
	#if HC05RxDMA
	/* NVIC 配置 ----------------------------------------------------------*/
	RxNVICInit(USARTBASE);
	#endif
	
	USART_ClearFlag(USARTBASE, USART_FLAG_TC);						//清除发送成功标志
	
}
/******************* (C) COPYRIGHT 2014 STMicroelectronics *****END OF FILE****/
