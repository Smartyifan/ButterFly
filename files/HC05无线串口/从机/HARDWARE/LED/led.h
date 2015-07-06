#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
//Mini STM32开发板
//LED驱动代码			 
//正点原子@ALIENTEK
//2010/5/27

//LED端口定义
#define LED0 PAout(8)// PA8
#define LED1 PDout(2)// PD2	

void LED_Init(void);//初始化		 				    
#endif

















