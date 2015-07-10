/**
  ******************************************************************************
  * @file    E:\ButterFly\Hardware\KS103\ks103.c
  * @author  ��һ��
  * @version V0.0
  * @date    2015-07-06 21:18:24
  * @brief   Blank
  ******************************************************************************
  * @attention
  * 2015-07-09 22:06:05
  * ̽����̣�
  * 1.ʹ��Read_KS10X_Data����ִ��̽��������ⲿ�жϡ�rangeΪ�����뾫�ȵ�ָ��
  * 2.ִ����������������Ҫ�ȴ���ʮms��̽����ɴ������ݣ�Ϊ�˼��ٳ����ռ���ʣ�
  *   ʹ���ⲿ�жϡ�SCL����̽�����ǰΪ�͵�ƽ��̽����ɺ�Ϊ�ߵ�ƽ�����жϳ�����
  *   ��λ̽����ɱ�־����ȡ���ݣ���������жϣ����ݴ���ڽṹ���С�
  *   �ⲿ�ж����ȼ�Ϊ��߼�0��
  * 2015-07-10 10:17:17
  * ʹ�÷�����
  * 1.��ks103iic.h������������صĺ궨��
  * 2.�ڳ�ʼ��������SDA��SCL������
  * 3.��д�����ⲿ�ж�(SCLInEXTI)���ⲿ�жϷ�����(EXTIx_IRQHandler)
  * 4.ÿ��̽��ʱ���ȵ���Read_KS10X_Data��֮���ȡKS103.detected��־λ����ΪSUCCESS
  *   ����Զ�ȡKS103.Height����
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
void SCLEXTIInit(KS103Str * KS103);										//SCL�ߵ��ⲿ�жϳ�ʼ�����ڶ�ȡ����ʱ�õ�
void SCLInEXTI(KS103Str * KS103);										//�����ⲿ�ж�ģʽ			
void Read_KS10X_Data(KS103Str * KS103, u8 range);						//��������ָ���ȡ�߶�	
void Single_WriteI2C(u8 SlaveAddress,u8 REG_Address,u8 REG_data);		//����I2C��һ���ֽ�
u8 Single_ReadI2C(u8 SlaveAddress,u8 REG_Address);						//����I2Cдһ���ֽ�
void Read_KS10X(KS103Str * KS103);										//��16λ����
void Change_Addr(u8 OldAdddr,u8 NewAddr);								//�����µ�ַ
/* Private functions ---------------------------------------------------------*/
/**
  *@brief   Function Name	
  *@param   None;
  *@retval  None
  */
ErrorStatus KS103Init(KS103Str * KS103){
	u8 Year,Week;
	
	I2CInit(KS103);		//I2C��ʼ��
	SCLEXTIInit(KS103);	//�ⲿ�жϳ�ʼ��
	
	KS103->Checked = ERROR;
	
	Year = Single_ReadI2C(KS103->SlaveAddress,0x00);			//��ȡ����ʱ����Ϣ
// 	HC05printf(&HC05,DMA1_Channel4,"Year = %x\r\n",Year);
	Week = Single_ReadI2C(KS103->SlaveAddress,0x01);
// 	HC05printf(&HC05,DMA1_Channel4,"Week = %x\r\n",Week);
	if(Week == 0x1f && Year == 0x0e)KS103->Checked = SUCCESS;			
	
// 	Single_WriteI2C(KS103->SlaveAddress,2,0x71);		//�ڶ�������,������USB���磬ִֻ��һ�Σ��ϵ籣������
// 	delay_ms(2000);
	
	return KS103->Checked;
}
 

/**
  *@brief   SCLInEXTI		//SCL�߽����ⲿ�ж�
  *@param   None
  *@retval    None
  */
void SCLInEXTI(KS103Str * KS103){
	GPIOF->CRL&=0xffffff0f;
	GPIOF->CRL|=0x00000080;		
	GPIOF->BRR = 0x0002;		//����ΪGPIOF.1��������

	EXTI->IMR|=0x0002;			//������1���ж�
}

/**
  *@brief   EXTI1_IRQHandler		//�ⲿ�жϷ�����
  *@param   None
  *@retval    None
  */
void EXTI1_IRQHandler(){
	
	EXTI_ClearITPendingBit(EXTI_Line1);    	//���1���жϱ�־
		
	EXTI->IMR&=~0x0002;					//����1���ϵ��ⲿ�ж�

	GPIOF->CRL &= 0xffffff0f;
	GPIOF->CRL |= 0x00000030;				//SCL�����������

	Read_KS10X(&KS103); 					//��ȡ����
		
	KS103.detected = SUCCESS;				//̽��ɹ�
}

/**
  *@brief   Read_KS10X_Data		//��������ָ���ȡ�߶�	
  *@param   None
  *@retval    None
  */
void Read_KS10X_Data(KS103Str * KS103,u8 range){	//��������ָ���ȡ�߶�		
	
	KS103->detected = ERROR;						//δ̽��ɹ�
	Single_WriteI2C(KS103->SlaveAddress,2,range);	//�߾���5m����̽��,�����ʱ��68ms
	
	SCLInEXTI(KS103);								//����ȴ��ⲿ�ж�״̬���ⲿ�ж��н�̽��ɹ���־��λ
}
void Single_WriteI2C(u8 SlaveAddress,u8 REG_Address,u8 REG_data){	//I2Cдһ���ֽ�      
    IIC_Start();                    //��ʼ�ź�
    IIC_Send_Byte(SlaveAddress);   	//�����豸��ַ+д�ź�
 
    while(IIC_Wait_Ack());
 
    IIC_Send_Byte(REG_Address);    //�ڲ��Ĵ�����ַ
 
    while(IIC_Wait_Ack());                         
                                    
    IIC_Send_Byte(REG_data);       //�ڲ��Ĵ�������
 
    while(IIC_Wait_Ack());                                 
    IIC_Stop();                     //����ֹͣ�ź�
}         

u8 Single_ReadI2C(u8 SlaveAddress,u8 REG_Address){		//����I2C��һ���ֽ�
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

void Read_KS10X(KS103Str * KS103){										//��16λ����
	KS103->Height=Single_ReadI2C(KS103->SlaveAddress,0x02);             //��8λ
	KS103->Height<<=8;
	KS103->Height+=Single_ReadI2C(KS103->SlaveAddress,0x03);                //��8λ
}


void Change_Addr(u8 OldAdddr,u8 NewAddr){			//�����µ�ַ
	Single_WriteI2C(OldAdddr,0x02,0x9a);            //ԭ��ַ
	delay_ms(1);
	Single_WriteI2C(OldAdddr,0x02,0x92);
	delay_ms(1);
	Single_WriteI2C(OldAdddr,0x02,0x9e);
	delay_ms(1);
	Single_WriteI2C(OldAdddr,0x02,NewAddr);
	delay_ms(100);
}
 


/**
  *@brief   SDAEXTIInit		SDA�ߵȴ�����ʱ���ⲿ�ж�
  *@param   None
  *@retval    None
  */
void SCLEXTIInit(KS103Str * KS103){
	/* ��ʼ���ṹ�嶨�� ------------------------------------------------*/
    EXTI_InitTypeDef EXTI_InitStructure;    
    NVIC_InitTypeDef NVIC_InitStructure;
	
	/* ����ʱ�� --------------------------------------------------------*/
	//GPIO������GPIOInit����ʹ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE); //ʹ��ʱ�Ӹ�������
	

	/* �ⲿ�жϳ�ʼ�� --------------------------------------------------*/
	GPIO_EXTILineConfig(KS103->I2C.ClkPortSrc,KS103->I2C.ClkPinSrc);   	
	EXTI_InitStructure.EXTI_Line = KS103->I2C.ClkPin;                     	
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;             //�ж��¼�
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;         	//�����ش���
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;                       //�ж���ʹ��
    EXTI_Init(&EXTI_InitStructure);                                 //��ʼ���ж�
	
	NVIC_InitStructure.NVIC_IRQChannel = KS103->I2C.ClkPinSrc+6;      //�жϺ�IRQn
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;       //��ռ���ȼ�0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;       //�����ȼ�0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);                                 //�жϹ�������ʼ��
	
	EXTI->IMR&=~(1<<KS103->I2C.ClkPinSrc);							//�������ϵ��ⲿ�ж�
}

/******************* (C) COPYRIGHT 2014 STMicroelectronics *****END OF FILE****/
