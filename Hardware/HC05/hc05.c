/**
  ******************************************************************************
  * @file    	E:\ButterFly\Hardware\HC05\hc05.c
  * @author  	��һ�����������ԭ�ӣ�
  * @version	V0.3
  * @date  		2015-07-05 15:59:29
  * @brief   	HC-05��������ģ��
  ******************************************************************************
  * @attention
  *	V0.0  2015-07-06 00:08:45 
  * ֻ��ʹ��USART1~3
  * ���ڽ����жϵ���ռ���ȼ�Ϊ1�������ȼ�Ϊ0��������RxNVICInit�������޸�
  * ��ͷ�ļ�������HC05TxDMA��HC05RxDMA��ֵ�����Ƿ��õ�DMA���䣨Ĭ��ʹ��DMA��
  * ��ͷ�ļ�������DMA���ͺͽ����ڴ�ĳ���
  * V0.3  2015-07-09 20:36:27 
  * ��HC05Str�ṹ���У������DMA�����뷢��ͨ���ţ����ڳ�ʼ�������ж��丳ֵ��
  * �Ա�HC05Printf������ʹ��
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
HC05Str HC05;		//HC05�ṹ��	
/* Private function prototypes -----------------------------------------------*/
void HC05printf(HC05Str * HC05,char* fmt,...);				//����DMA�����printf
void RxDataHandler(u8 * Data,u16 Len);						//�������ݴ�����
void UARTxDMASend(HC05Str * HC05,u16 Len);					//����һ�δ��ڵ�DMA����
void HC05UartInit(USART_TypeDef * USARTBASE,u32 BaudRate);		//���ڳ�ʼ��
void HC05DMAInit(HC05Str * HC05);								//DMA��ʼ��
void HC05RxNVICInit(USART_TypeDef * USARTBASE);					//NVIC��ʼ��
void HC05GPIOInit(HC05Str * HC05);								//GPIO��ʼ��
/* Private functions ---------------------------------------------------------*/
/**
  *@brief   HC05Init
  *@param   None;
  *@retval  None
  */
ErrorStatus HC05Init(HC05Str * HC05){
	u8 timeout = 10,wait;		//��鳬ʱ
	
	HC05->Checked = ERROR;		//���HC05ģ��ɹ���־
	/* ȷ��DMAͨ���� ---------------------------------------------------------------------------------------------*/
	if(HC05->USARTBASE == USART1){
		HC05->DMAChannelTx = DMA1_Channel4;	//Txͨ�� 
		HC05->DMAChannelRx = DMA1_Channel5;	//Rxͨ��
	}else if(HC05->USARTBASE == USART2){
		HC05->DMAChannelTx = DMA1_Channel7;	//Txͨ�� 
		HC05->DMAChannelRx = DMA1_Channel6;	//Rxͨ��
	}else if(HC05->USARTBASE == USART3){
		HC05->DMAChannelTx = DMA1_Channel2;	//Txͨ�� 
		HC05->DMAChannelRx = DMA1_Channel3;	//Rxͨ��
	}

	
	HC05GPIOInit(HC05);
	HC05UartInit(HC05->USARTBASE,9600);		//��ʼ���ж�
	
	/* DMA��ʼ�� ---------------------------------------------------*/
	#if HC05TxDMA	//��ʹ��TxDMA����	
	USART_DMACmd(HC05->USARTBASE,USART_DMAReq_Tx,ENABLE);
	#endif
	#if HC05RxDMA	//��ʹ��RxDMA����	
	USART_DMACmd(HC05->USARTBASE,USART_DMAReq_Rx,ENABLE);
	#endif
	
	HC05DMAInit(HC05);
	DMA_SetCurrDataCounter(HC05->DMAChannelTx,0);		//TxDMAͨ����������գ�ʹ֮��ִ�е�һ��HC05Pintf����
	
	while(timeout--){
		HC05Key = 1;			//KEY=1,����ATģʽ
		delay_ms(10);
		
		HC05printf(HC05,"AT\r\n");
		HC05Key = 0;			//KEY = 0,�˳�ATģʽ
		
		for(wait = 0;wait < 10;wait ++){			//�ȴ�50ms����HC05ģ��Ļ�Ӧ
			if(HC05->Checked == SUCCESS)return HC05->Checked;;	//�����ճɹ����˳�Init����	
			delay_ms(10);					
		}
	}
	
	
	return ERROR;		//���ؼ���־
}

/**
  *@brief   USART1_IRQHandler	USART1�жϷ�����
  *@param   None
  *@retval  None
  *@brief   �����жϺ�ر�DMA���䣬�����ٴδ��䣬��Ҫ�����ݴ�����ɺ��ٴδ�
  */
void USART1_IRQHandler(){
	u32 reg = 0;
	HC05.RxLen = 0;
	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET) {	//��������ж�
		DMA_Cmd(DMA1_Channel5,DISABLE); 					//�رմ���1�Ľ���DMAͨ��
		reg = USART1->SR;
		reg = USART1->DR;		//����������������IDLE�жϱ�־
		
		HC05.RxLen = HC05RxLen - DMA_GetCurrDataCounter(DMA1_Channel5);			//������յ��ĳ���
		
		if(HC05.RxLen == 4 && HC05.RxData[0] == 'O' && HC05.RxData[1] == 'K')	//���յ�'OK\r\n'
			HC05.Checked = SUCCESS;												//��⵽HC-05	
		
		DMA_SetCurrDataCounter(DMA1_Channel5,HC05RxLen);		//�ٴ����ý��ճ���
		DMA_Cmd(DMA1_Channel5,ENABLE); 							//ʹ�ܴ���5�Ľ���DMAͨ��
	} 
}


/**
  *@brief   
  *@param   None
  *@retval    None
  */
void HC05printf(HC05Str * HC05,char* fmt,...){
	va_list ap;
	va_start(ap,fmt);
	
	vsprintf((char*)HC05->TxData,fmt,ap);
	va_end(ap);
	UARTxDMASend(HC05,strlen((const char*)HC05->TxData)); 	//ͨ��dma���ͳ�ȥ
}


/**
  *@brief   UARTxDMASend		����һ�δ��ڵ�DMA����
  *@param   DMA_Channel_TypeDef*DMA_CHx		//DMAͨ��
  * 		u16 Len							//���ݳ���
  *@retval    None
  */
void UARTxDMASend(HC05Str * HC05,u16 Len){
	
	while(HC05->DMAChannelTx->CNDTR!=0);		//�ȴ�Txͨ���������   
	while((HC05->USARTBASE->SR&0X40)==0);	//�ȴ����ڷ������

	HC05->DMAChannelTx->CCR&=~1;      //�ر�DMA���� 
	HC05->DMAChannelTx->CNDTR=Len;    		//DMA,���������� 
	HC05->DMAChannelTx->CCR|=1;         //����DMA����
}



/**
  *@brief   GPIOInit	GPIO��ʼ��
  *@param   None
  *@retval    None
  */
void HC05GPIOInit(HC05Str * HC05){
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* ����ʱ�� ------------------------------------------------------------------------------*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA<<((HC05->KeyBase-0x40010800)/0x00000400),ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA<<((HC05->LEDBase-0x40010800)/0x00000400),ENABLE);
	
	/* Key Pin Init ------------------------------------------------------------*/
	GPIO_InitStructure.GPIO_Pin= HC05->KeyPin; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//50MHz���Ƶ��
	GPIO_Init((GPIO_TypeDef *)HC05->KeyBase,&GPIO_InitStructure);	//��ʼ��
	
	/* LED Pin Init ------------------------------------------------------------*/
	GPIO_InitStructure.GPIO_Pin= HC05->LEDPin; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;					//��������
	GPIO_Init((GPIO_TypeDef *)HC05->LEDBase,&GPIO_InitStructure);	//��ʼ��
}
/**
  *@brief   
  *@param   None
  *@retval    None
  */
void HC05DMAInit(HC05Str * HC05){
	DMA_InitTypeDef DMA_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//ʹ��DMA����
	delay_us(5);

	#if HC05TxDMA								//��ʹ��TxDMA����	
	/* UARTx  Tx DMAͨ����ʼ�� ---------------------------------------------------------------------------------*/
    DMA_DeInit(HC05->DMAChannelTx);   									//��DMA��ͨ��x�Ĵ�������Ϊȱʡֵ
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(HC05->USARTBASE)->DR);  //DMA����USART->DR����ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&HC05->TxData;  		//DMA�ڴ����ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST; 			 		//���ݴ��䷽�򣬴��ڴ��ȡ���͵�����
	DMA_InitStructure.DMA_BufferSize = HC05TxLen;  						//DMAͨ����DMA����Ĵ�С
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  	//�����ַ�Ĵ�������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  			//�ڴ��ַ�Ĵ�������
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //���ݿ��Ϊ8λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; 	//���ݿ��Ϊ8λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  						//��������������ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_High; 				//DMAͨ�� xӵ�������ȼ� 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  						//DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
	DMA_Init(HC05->DMAChannelTx, &DMA_InitStructure);  					//����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��USARTx_Tx_DMA_Channex����ʶ�ļĴ���
	
	
	#endif
	
	#if HC05RxDMA							//��ʹ��RxDMA����	
	/* UARTx  Rx DMAͨ����ʼ�� ---------------------------------------------------------------------------------*/
	DMA_DeInit(HC05->DMAChannelRx);   									//��DMA��ͨ��x�Ĵ�������Ϊȱʡֵ
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(HC05->USARTBASE)->DR);  //DMA����USART->DR����ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&HC05->RxData;  		//DMA�ڴ����ַ  RxData����
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; 			 		//���ݴ��䷽�򣬴������ȡ�����͵��ڴ�
	DMA_InitStructure.DMA_BufferSize = HC05RxLen;  						//DMAͨ����DMA����Ĵ�С
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  	//�����ַ�Ĵ�������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  			//�ڴ��ַ�Ĵ�������
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //���ݿ��Ϊ8λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; 	//���ݿ��Ϊ8λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  						//��������������ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh; 			//DMAͨ�� xӵ�г������ȼ� 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  						//DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
	DMA_Init(HC05->DMAChannelRx, &DMA_InitStructure);  					//����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��USARTx_Rx_DMA_Channex����ʶ�ļĴ���
	#endif
}

#if HC05RxDMA
/**
  *@brief   RxNVICInit
  *@param   USART_TypeDef * USARTBASE	//���ں�
  *@retval    None
  */
void HC05RxNVICInit(USART_TypeDef * USARTBASE){
	IRQn_Type IRQChannel;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* ȷ�ϲ��� ---------------------------------------------------------------*/
	if(USARTBASE == USART1){
		IRQChannel = USART1_IRQn;
	}else if(USARTBASE == USART2){
		IRQChannel = USART2_IRQn;
	}else if(USARTBASE == USART3){
		IRQChannel = USART3_IRQn;
	}
	
	/* NVIC��ʼ�� -------------------------------------------------------------*/
	NVIC_InitStructure.NVIC_IRQChannel = IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;		//��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 				//�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					//ʹ���жϺ�
	NVIC_Init(&NVIC_InitStructure);									//��ʼ��NVIC
}
#endif


/**
  *@brief   UartInit	HC-05ʹ�õĴ��ڳ�ʼ��
  *@param   USART_TypeDef * USARTBASE	���ں�
  *			u32 BaudRate	������
  *@retval    None
  */
void HC05UartInit(USART_TypeDef * USARTBASE,u32 BaudRate){
	u16 GPIO_Pin_Tx;			//Tx����
	u16 GPIO_Pin_Rx;			//Rx����
	GPIO_TypeDef * GPIOBase;	//GPIOBase
	
	/* �����ʼ���ṹ�� --------------------------------------------------------*/
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	/* ���ò��� ----------------------------------------------------------------*/
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

	/* ʹ��ʱ�� ----------------------------------------------------------------*/
	if(USARTBASE == USART1){
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);	//ʹ��UART1ʱ��
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��GPIOAʱ��
	}else if(USARTBASE == USART2){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);	//ʹ��UART2ʱ��
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��GPIOAʱ��
	}else if(USARTBASE == USART3){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);	//ʹ��UART3ʱ��
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//ʹ��GPIOBʱ��
	}

 	USART_DeInit(USARTBASE);  								//��λ����

	/* GPIO�˿����� -------------------------------------------------------------*/
	//USARTx_TX   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_Tx; 			//TXD
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		//�����������
    GPIO_Init(GPIOBase, &GPIO_InitStructure); 			//��ʼ��TxD

	//USARTx_RX	  
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_Rx;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//��������
    GPIO_Init(GPIOBase, &GPIO_InitStructure);  				//��ʼ��RxD

	/* USARTx ��ʼ������ --------------------------------------------------*/
	USART_InitStructure.USART_BaudRate = BaudRate;					//������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//�ֳ�Ϊ8λ���ݸ�ʽ	��Ĭ��ģʽ��
	USART_InitStructure.USART_StopBits = USART_StopBits_1;			//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;				//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
    USART_Init(USARTBASE, &USART_InitStructure); 					//��ʼ������x
	
	/* �ж����� -----------------------------------------------------------*/
	USART_ITConfig(USARTBASE,USART_IT_TC,DISABLE);
	USART_ITConfig(USARTBASE,USART_IT_RXNE,DISABLE);
    USART_ITConfig(USARTBASE,USART_IT_IDLE,ENABLE);					//���ڿ����ж�
	
	
	USART_Cmd(USARTBASE, ENABLE);                  					//ʹ�ܴ���x
	
	#if HC05RxDMA
	/* NVIC ���� ----------------------------------------------------------*/
	HC05RxNVICInit(USARTBASE);
	#endif
	
	USART_ClearFlag(USARTBASE, USART_FLAG_TC);						//������ͳɹ���־
	
}
/******************* (C) COPYRIGHT 2014 STMicroelectronics *****END OF FILE****/
