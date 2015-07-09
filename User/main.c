/**
  ******************************************************************************
  * @file    STM32工程/工程模版/User
  * @author  贾一帆
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
  *@brief   SetParam		//设置参数
  *@param   None
  *@retval    None
  */
void SetParam(void){
	/* HC05 -------------------------------------------------------*/
	HC05.USARTBASE = USART1;		//使用串口1
	
	HC05.KeyBase = GPIOD_BASE;		//Key引脚
	HC05.KeyPin = GPIO_Pin_1;

	HC05.LEDBase = GPIOD_BASE;		//LED引脚
	HC05.LEDPin = GPIO_Pin_0;
}
/**
  *@brief   Initial
  *@param   None
  *@retval  None
  */
void Initial(void)
{
	SetParam();
	
	NVIC_Configuration();	//中断分组2
	delay_init();
    
	LEDInit();							//LED初始化
	if(HC05Init(&HC05) == SUCCESS){		//HC05初始化
		HC05printf(&HC05,DMA1_Channel4,"HC05 Connected...\r\n");
	}else {
		HC05printf(&HC05,DMA1_Channel4,"HC05 Disconnected...\r\n");
	}
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
