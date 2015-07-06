#include "sys.h"
#include "usart.h"		
#include "usart2.h"		
#include "delay.h"	
#include "led.h"    	 	  
#include "timer.h"	 	 	 
#include "lcd.h"
#include "hc05.h"	 	 
#include "string.h"	   
//ALIENTEK MiniSTM32��������չʵ��22
//ATK-HC05��������ģ��ʵ��  
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾ 

//��ʾATK-HC05ģ�������״̬
void HC05_Role_Show(void)
{
	if(HC05_Get_Role()==1)LCD_ShowString(30,140,200,16,16,"ROLE:Master");	//����
	else LCD_ShowString(30,140,200,16,16,"ROLE:Slave ");			 		//�ӻ�
}
//��ʾATK-HC05ģ�������״̬
void HC05_Sta_Show(void)
{												 
	if(HC05_LED)LCD_ShowString(120,140,120,16,16,"STA:Connected ");			//���ӳɹ�
	else LCD_ShowString(120,140,120,16,16,"STA:Disconnect");	 			//δ����				 
}	  
int main(void)
{	 
	u8 t;
	u8 key;
	u8 sendmask=0;
	u8 sendcnt=0;
	u8 sendbuf[20];	  
	u8 reclen=0;  
   	Stm32_Clock_Init(9);	//ϵͳʱ������
	delay_init(72);			//��ʱ��ʼ��
	uart_init(72,9600); 	//����1��ʼ��Ϊ9600
	LED_Init();				//��ʼ����LED���ӵ�Ӳ���ӿ�
    LCD_Init();				//��ʼ��LCD 
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
			LCD_ShowString(30+40,160,200,16,16,sendbuf);	//��ʾ��������	
			u2_printf("ALIENTEK HC05 %d\r\n",sendcnt);		//���͵�����ģ��
			sendcnt++;
			if(sendcnt>99)sendcnt=0;
			HC05_Sta_Show();  	  
			t=0;
			LED0=!LED0; 	     
		}	  
		if(USART2_RX_STA&0X8000)			//���յ�һ��������
		{
			LCD_Fill(30,200,240,320,WHITE);	//�����ʾ
 			reclen=USART2_RX_STA&0X7FFF;	//�õ����ݳ���
		  	USART2_RX_BUF[reclen]=0;	 	//���������
			if(reclen==9||reclen==8) 		//����DS1���
			{
				if(strcmp((const char*)USART2_RX_BUF,"+LED1 ON")==0)LED1=0;	//��LED1
				if(strcmp((const char*)USART2_RX_BUF,"+LED1 OFF")==0)LED1=1;//�ر�LED1
			}
 			LCD_ShowString(30,200,209,119,16,USART2_RX_BUF);//��ʾ���յ�������
 			USART2_RX_STA=0;	 
		}	 															     				   
		t++;	
	}											    
}




















