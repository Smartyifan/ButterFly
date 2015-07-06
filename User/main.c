/**
  ******************************************************************************
  * @file    STM32����/����ģ��/User
  * @author  ��һ��
  * @version V3.5.0
  * @date    2014-10-4
  * @brief   Main program body
  ******************************************************************************
  * @attention
  * No attention
  ******************************************************************************
  */  

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */
#include "sys/sys.h"  
#include "delay/delay.h"

#include "HC05/hc05.h"
#include "LED/led.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  *@brief   SetParam		//���ò���
  *@param   None
  *@retval    None
  */
void SetParam(){
	/* HC05 -------------------------------------------------------*/
	HC05.USARTBASE = USART2;		//ʹ�ô���2
	
	HC05.KeyBase = GPIOC_BASE;		//Key����
	HC05.KeyPin = GPIO_Pin_7;

	HC05.LEDBase = GPIOC_BASE;		//LED����
	HC05.LEDPin = GPIO_Pin_8;
	
	
}
/**
  *@brief   Initial
  *@param   None;
  *@retval  None
  */
void Initial()
{
	SetParam();
	
	NVIC_Configuration();	//�жϷ���2
	delay_init();
    
	LEDInit();							//LED��ʼ��
	if(HC05Init(&HC05) == SUCCESS){		//HC05��ʼ��
		GLED = 0;
	}else RLED = 0;
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
    Initial();
    while(1)
    {
        
    }
}

/******************* (C) COPYRIGHT 2014 STMicroelectronics *****END OF FILE****/
