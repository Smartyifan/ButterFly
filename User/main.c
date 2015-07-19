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
#include "KS103/ks103.h"
#include "UpperMachine/upmac.h"
#include "JY901/jy901.h"
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
	
	/* KS103 ------------------------------------------------------*/
	KS103.I2C.ClkGPIOBase = GPIOF_BASE;
	KS103.I2C.ClkPin = GPIO_Pin_1;
	
	KS103.I2C.SDAGPIOBase = GPIOF_BASE;
	KS103.I2C.SDAPin = GPIO_Pin_0;
	
	KS103.SlaveAddress = SlaveAddress1;		//设备地址
	
	/* JY901 ------------------------------------------------------*/
	JY901.USARTBASE = USART2;		//使用串口2
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
	if(HC05Init(&HC05) == SUCCESS){		//HC05初始化，并检测模块是否存在
		HC05printf(&HC05,"HC05 Connected...\r\n");
	}else {
		HC05printf(&HC05,"HC05 Disconnected...\r\n");
	}
	
	delay_ms(20);
	
	if(KS103Init(&KS103) == SUCCESS)			//KS103初始化，并检测KS103模块
		HC05printf(&HC05,"KS103 Connected...\r\n");
	else
		HC05printf(&HC05,"KS103 Disonnected...\r\n");
	delay_ms(20);
	
	JY901Init(&JY901);			//九轴姿态传感模块 JY-901 初始化
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
// 		Read_KS10X_Data(&KS103,0xb4);
// 		while(KS103.detected != SUCCESS);

// 		HC05printf(&HC05,"Height = %d mm\r\n",KS103.Height);
// 		SimplePlotSend(&HC05,(float)KS103.Height,0,0,0);		//画图
// 		delay_ms(100);
    }
}

/******************* (C) COPYRIGHT 2014 STMicroelectronics *****END OF FILE****/
