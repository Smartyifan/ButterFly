#include "sys.h"
#include "usart.h"		
#include "usart2.h"		
#include "delay.h"	
#include "led.h"    	 	  
#include "timer.h"	 	 	 
#include "lcd.h"
#include "hc05.h"	 	 
#include "string.h"	   
//ALIENTEK MiniSTM32开发板扩展实验22
//ATK-HC05蓝牙串口模块实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司 

//显示ATK-HC05模块的主从状态
void HC05_Role_Show(void)
{
	if(HC05_Get_Role()==1)LCD_ShowString(30,140,200,16,16,"ROLE:Master");	//主机
	else LCD_ShowString(30,140,200,16,16,"ROLE:Slave ");			 		//从机
}
//显示ATK-HC05模块的连接状态
void HC05_Sta_Show(void)
{												 
	if(HC05_LED)LCD_ShowString(120,140,120,16,16,"STA:Connected ");			//连接成功
	else LCD_ShowString(120,140,120,16,16,"STA:Disconnect");	 			//未连接				 
}	  
int main(void)
{	 
	u8 t;
	u8 key;
	u8 sendmask=0;
	u8 sendcnt=0;
	u8 sendbuf[20];	  
	u8 reclen=0;  
   	Stm32_Clock_Init(9);	//系统时钟设置
	delay_init(72);			//延时初始化
	uart_init(72,9600); 	//串口1初始化为9600
	LED_Init();				//初始化与LED连接的硬件接口
    LCD_Init();				//初始化LCD 
	POINT_COLOR=RED; 
	HC05_Init();	
  	LCD_ShowString(30,160,200,16,16,"Send:");	
	LCD_ShowString(30,180,200,16,16,"Receive:");	
	POINT_COLOR=BLUE;
	HC05_Role_Show();	  
 	while(1) 
	{		
		if(t==50)
		{
			sprintf((char*)sendbuf,"ALIENTEK HC05 %d\r\n",sendcnt);
			LCD_ShowString(30+40,160,200,16,16,sendbuf);	//显示发送数据	
			u2_printf("ALIENTEK HC05 %d\r\n",sendcnt);		//发送到蓝牙模块
			sendcnt++;
			if(sendcnt>99)sendcnt=0;
			HC05_Sta_Show();  	  
			t=0;
			LED0=!LED0; 	     
		}	  
		if(USART2_RX_STA&0X8000)			//接收到一次数据了
		{
			LCD_Fill(30,200,240,320,WHITE);	//清除显示
 			reclen=USART2_RX_STA&0X7FFF;	//得到数据长度
		  	USART2_RX_BUF[reclen]=0;	 	//加入结束符
			if(reclen==9||reclen==8) 		//控制DS1检测
			{
				if(strcmp((const char*)USART2_RX_BUF,"+LED1 ON")==0)LED1=0;	//打开LED1
				if(strcmp((const char*)USART2_RX_BUF,"+LED1 OFF")==0)LED1=1;//关闭LED1
			}
 			LCD_ShowString(30,200,209,119,16,USART2_RX_BUF);//显示接收到的数据
 			USART2_RX_STA=0;	 
		}	 															     				   
		t++;	
	}											    
}




















