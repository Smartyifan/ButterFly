/**
  ******************************************************************************
  * @file    ����ʵ��/Hardware/KEY
  * @author  ��һ��
  * @version V3.5.0
  * @date    2014-10-4
  * @brief   Mini������������������
  ******************************************************************************
  * @attention
  * Key0 -> PA.13
  * Key1 -> PA.15
  * WakeUp -> PA.0 
  ******************************************************************************
  */  
  
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */
#include "sys/sys.h"
#include "delay/delay.h"

#include "KEY/key.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/**
  *@brief   KeyInit	 ��PA.0 PA.13 PA.15����Ϊ����ģʽ
  *@param   None
  *@retval  None
  */
void KeyInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;    //����GPIO��ʼ���ṹ��
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);    //ʹ��GPIOA��ʱ��

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;    			//PA.0
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;           //�������룬����ʱΪ�͵�ƽ
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;       //PA.0
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;   //�������룬����ʱΪ�ߵ�ƽ
    GPIO_Init(GPIOA,&GPIO_InitStructure);
}
 
/**
  *@brief   KeyScan ����ɨ��
  *@param   None                      0     1     2     3     4     5     6    7    8
  *@retval  u8 KeyVal  0~2λ�ֱ��Ӧ KEY0  KEY1  WKUP   X     X     X     X    X    X
  */
u8 KeyScan(void)
{
    
    if(KEY0==0){          //�а�������
        delay_ms(100);                       //ȥ��
        if(KEY0 == 0) return 0x01;          //��KEY0���£�����0b0000_0001
    }
    
    return 0;                               //���ް������£�����0
}
/******************* (C) COPYRIGHT 2014 STMicroelectronics *****END OF FILE****/
