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
void SetParam(){
	/* HC05 -------------------------------------------------------*/
	HC05.USARTBASE = USART2;		//使用串口2
	
	HC05.KeyBase = GPIOC_BASE;		//Key引脚
	HC05.KeyPin = GPIO_Pin_7;

	HC05.LEDBase = GPIOC_BASE;		//LED引脚
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
	
	NVIC_Configuration();	//中断分组2
	delay_init();
    
	LEDInit();							//LED初始化
	if(HC05Init(&HC05) == SUCCESS){		//HC05初始化
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
