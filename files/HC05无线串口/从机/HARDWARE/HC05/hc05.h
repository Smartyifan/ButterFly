#ifndef __HC05_H
#define __HC05_H	 
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//ATK-HC05����ģ����������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2013/2/22
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	   

#define HC05_KEY  	PCout(4) 	//��������KEY�ź�
#define HC05_LED  	PCin(5)		//��������״̬�ź�
  
u8 HC05_Init(void);
void HC05_CFG_CMD(u8 *str);
u8 HC05_Get_Role(void);
u8 HC05_Set_Cmd(u8* atstr);	   
#endif  
















