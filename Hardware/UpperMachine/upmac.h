/**
  ******************************************************************************
  * @file    
  * @author  
  * @version 
  * @date    
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */  
  
#ifndef  _UPMAC_H
#define  _UPMAC_H
/* Includes ------------------------------------------------------------------*/
#include <stm32f10x.h>
#include "sys/sys.h"
#include "HC05/hc05.h"
/* Define --------------------------------------------------------------------*/
/* TypeDefine ----------------------------------------------------------------*/

/* extern Variables ----------------------------------------------------------*/

/* extern function------------------------------------------------------------*/
extern void SimplePlotSend(HC05Str * HC05,float A,float B,float C,float D);	 				    
#endif
/******************* (C) COPYRIGHT 2014 STMicroelectronics *****END OF FILE****/