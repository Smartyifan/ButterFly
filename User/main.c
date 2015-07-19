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
  *@brief   SetParam		//���ò���
  *@param   None
  *@retval    None
  */
void SetParam(void){
	/* HC05 -------------------------------------------------------*/
	HC05.USARTBASE = USART1;		//ʹ�ô���1
	
	HC05.KeyBase = GPIOD_BASE;		//Key����
	HC05.KeyPin = GPIO_Pin_1;

	HC05.LEDBase = GPIOD_BASE;		//LED����
	HC05.LEDPin = GPIO_Pin_0;
	
	/* KS103 ------------------------------------------------------*/
	KS103.I2C.ClkGPIOBase = GPIOF_BASE;
	KS103.I2C.ClkPin = GPIO_Pin_1;
	
	KS103.I2C.SDAGPIOBase = GPIOF_BASE;
	KS103.I2C.SDAPin = GPIO_Pin_0;
	
	KS103.SlaveAddress = SlaveAddress1;		//�豸��ַ
	
	/* JY901 ------------------------------------------------------*/
	JY901.USARTBASE = USART2;		//ʹ�ô���2
}
/**
  *@brief   Initial
  *@param   None
  *@retval  None
  */
void Initial(void)
{
	SetParam();
	
	NVIC_Configuration();	//�жϷ���2
	delay_init();
    
	LEDInit();							//LED��ʼ��
	if(HC05Init(&HC05) == SUCCESS){		//HC05��ʼ���������ģ���Ƿ����
		HC05printf(&HC05,"HC05 Connected...\r\n");
	}else {
		HC05printf(&HC05,"HC05 Disconnected...\r\n");
	}
	
	delay_ms(20);
	
	if(KS103Init(&KS103) == SUCCESS)			//KS103��ʼ���������KS103ģ��
		HC05printf(&HC05,"KS103 Connected...\r\n");
	else
		HC05printf(&HC05,"KS103 Disonnected...\r\n");
	delay_ms(20);
	
	JY901Init(&JY901);			//������̬����ģ�� JY-901 ��ʼ��
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
// 		SimplePlotSend(&HC05,(float)KS103.Height,0,0,0);		//��ͼ
// 		delay_ms(100);
    }
}

/******************* (C) COPYRIGHT 2014 STMicroelectronics *****END OF FILE****/
