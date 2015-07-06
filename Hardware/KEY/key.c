/**
  ******************************************************************************
  * @file    按键实验/Hardware/KEY
  * @author  贾一帆
  * @version V3.5.0
  * @date    2014-10-4
  * @brief   Mini开发板的三个按键检测
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
  *@brief   KeyInit	 将PA.0 PA.13 PA.15配置为输入模式
  *@param   None
  *@retval  None
  */
void KeyInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;    //定义GPIO初始化结构体
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);    //使能GPIOA的时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;    			//PA.0
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;           //上拉输入，按下时为低电平
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;       //PA.0
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;   //下拉输入，按下时为高电平
    GPIO_Init(GPIOA,&GPIO_InitStructure);
}
 
/**
  *@brief   KeyScan 按键扫描
  *@param   None                      0     1     2     3     4     5     6    7    8
  *@retval  u8 KeyVal  0~2位分别对应 KEY0  KEY1  WKUP   X     X     X     X    X    X
  */
u8 KeyScan(void)
{
    
    if(KEY0==0){          //有按键按下
        delay_ms(100);                       //去抖
        if(KEY0 == 0) return 0x01;          //若KEY0按下，返回0b0000_0001
    }
    
    return 0;                               //若无按键按下，返回0
}
/******************* (C) COPYRIGHT 2014 STMicroelectronics *****END OF FILE****/
