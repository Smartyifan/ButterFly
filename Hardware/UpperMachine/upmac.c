/**
  ******************************************************************************
  * @file    	E:\ButterFly\Hardware\UpperMachine\upmac.c
  * @author  	贾一帆
  * @version	V0.0
  * @date  		2015-07-10 10:38:53
  * @brief   	用于与UpperMachine上位机通讯
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */  

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */
#include "sys/sys.h"
#include "HC05/hc05.h"
#include "UpperMachine/upmac.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  *@brief   Initial
  *@param   None;
  *@retval  None
  */
/**
  *@brief   
  *@param   None
  *@retval    None
  */
void SimplePlotSend(HC05Str * HC05,float A,float B,float C,float D){
	union {
		float f;
		char c[4];
	} temp;

	HC05->TxData[0] = 0xAA;
	HC05->TxData[1] = 0xEE;
	
	temp.f = A;
	HC05->TxData[2] = temp.c[0];
	HC05->TxData[3] = temp.c[1];
	HC05->TxData[4] = temp.c[2];
	HC05->TxData[5] = temp.c[3];

	temp.f = B;
	HC05->TxData[6] = temp.c[0];
	HC05->TxData[7] = temp.c[1];
	HC05->TxData[8] = temp.c[2];
	HC05->TxData[9] = temp.c[3];
	
	temp.f = C;
	HC05->TxData[10] = temp.c[0];
	HC05->TxData[11] = temp.c[1];
	HC05->TxData[12] = temp.c[2];
	HC05->TxData[13] = temp.c[3];
	
	temp.f = D;
	HC05->TxData[14] = temp.c[0];
	HC05->TxData[15] = temp.c[1];
	HC05->TxData[16] = temp.c[2];
	HC05->TxData[17] = temp.c[3];
	UARTxDMASend(HC05,18);
}

/******************* (C) COPYRIGHT 2014 STMicroelectronics *****END OF FILE****/
