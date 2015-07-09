/**
  ******************************************************************************
  * @file    STM32����/����ģ��/Hardware
  * @author  ��һ��
  * @version V3.5.0
  * @date    2014-10-4
  * @brief   LED program
  ******************************************************************************
  * @attention
  * Red LED -> PA.8
  * Green LED -> PD.2
  ******************************************************************************
  */  
  
/* Includes ------------------------------------------------------------------*/
#include "LED/led.h"

/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/**
  *@brief   ��ʼ��PA8��PD2Ϊ�����.��ʹ���������ڵ�ʱ��		
  *@param   None;
  *@retval  None
  */
void LEDInit(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);	 //ʹ��PA,PD�˿�ʱ��
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				 //LED0-->PB.0 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOE, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.0
	
 GPIO_SetBits(GPIOE,GPIO_Pin_8);						 //PB.0 �����

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				 //LED0-->PC.0 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOE, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOC.0
	
 GPIO_SetBits(GPIOE,GPIO_Pin_10);						 //PC.0 �����
}
 
/******************* (C) COPYRIGHT 2014 STMicroelectronics *****END OF FILE****/
