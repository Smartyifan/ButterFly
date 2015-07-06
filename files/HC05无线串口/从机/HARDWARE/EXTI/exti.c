#include "exti.h"
#include "led.h"
#include "key.h"
#include "delay.h"
#include "usart.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//Mini STM32开发板
//外部中断 驱动代码			   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2010/12/01  
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved	  
////////////////////////////////////////////////////////////////////////////////// 	  
 

//外部中断0服务程序
void EXTI0_IRQHandler(void)
{
	delay_ms(10);//消抖
	if(KEY2==1)	 //按键2
	{
		LED0=!LED0;
		LED1=!LED1;	
	}		 
	EXTI->PR=1<<0;  //清除LINE0上的中断标志位  
}

//外部中断15~10服务程序
void EXTI15_10_IRQHandler(void)
{			
	delay_ms(10);    //消抖			 
	if(KEY0==0)      //按键0
	{
		LED0=!LED0;
	}else if(KEY1==0)//按键1
	{
		LED1=!LED1;
	}
	EXTI->PR=1<<13;     //清除LINE13上的中断标志位  
	EXTI->PR=1<<15;     //清除LINE15上的中断标志位  
}
//外部中断初始化程序
//初始化PA0,PA13,PA15为中断输入.
void EXTIX_Init(void)
{
	RCC->APB2ENR|=1<<2;     //使能PORTA时钟
	JTAG_Set(JTAG_SWD_DISABLE);//关闭JTAG和SWD   

	GPIOA->CRL&=0XFFFFFFF0;//PA0设置成输入	  
	GPIOA->CRL|=0X00000008;   
	GPIOA->CRH&=0X0F0FFFFF;//PA13,15设置成输入	  
	GPIOA->CRH|=0X80800000; 				   
	GPIOA->ODR|=1<<13;	   //PA13上拉,PA0默认下拉
	GPIOA->ODR|=1<<15;	   //PA15上拉

	Ex_NVIC_Config(GPIO_A,0,RTIR); //上升沿触发
	Ex_NVIC_Config(GPIO_A,13,FTIR);//下降沿触发
	Ex_NVIC_Config(GPIO_A,15,FTIR);//下降沿触发

	MY_NVIC_Init(2,2,EXTI0_IRQChannel,2);    //抢占2，子优先级2，组2
	MY_NVIC_Init(2,1,EXTI15_10_IRQChannel,2);//抢占2，子优先级1，组2	   
}












