#include "usmart.h"
#include "usart.h"
#include "sys.h"

//#include "timer.h"	      
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2012/9/17
//�汾��V2.9
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2011-2021
//All rights reserved
//********************************************************************************
//����˵��
//V1.4
//�����˶Բ���Ϊstring���͵ĺ�����֧��.���÷�Χ������.
//�Ż����ڴ�ռ��,��̬�ڴ�ռ��Ϊ79���ֽ�@10������.��̬��Ӧ���ּ��ַ�������
//V2.0 
//1,�޸���listָ��,��ӡ�������������ʽ.
//2,������idָ��,��ӡÿ����������ڵ�ַ.
//3,�޸��˲���ƥ��,֧�ֺ��������ĵ���(������ڵ�ַ).
//4,�����˺��������Ⱥ궨��.	
//V2.1 20110707		 
//1,����dec,hex����ָ��,�������ò�����ʾ����,��ִ�н���ת��.
//ע:��dec,hex����������ʱ��,���趨��ʾ��������.�����������ʱ��,��ִ�н���ת��.
//��:"dec 0XFF" ��Ὣ0XFFתΪ255,�ɴ��ڷ���.
//��:"hex 100" 	��Ὣ100תΪ0X64,�ɴ��ڷ���
//2,����usmart_get_cmdname����,���ڻ�ȡָ������.
//V2.2 20110726	
//1,������void���Ͳ����Ĳ���ͳ�ƴ���.
//2,�޸�������ʾ��ʽĬ��Ϊ16����.
//V2.3 20110815
//1,ȥ���˺�����������"("������.
//2,�������ַ��������в�����"("��bug.
//3,�޸��˺���Ĭ����ʾ������ʽ���޸ķ�ʽ. 
//V2.4 20110905
//1,�޸���usmart_get_cmdname����,������������������.����������������ʱ����������.
//2,����USMART_ENTIM2_SCAN�궨��,���������Ƿ�ʹ��TIM2��ʱִ��scan����.
//V2.5 20110930
//1,�޸�usmart_init����Ϊvoid usmart_init(u8 sysclk),���Ը���ϵͳƵ���Զ��趨ɨ��ʱ��.(�̶�100ms)
//2,ȥ����usmart_init�����е�uart_init����,���ڳ�ʼ���������ⲿ��ʼ��,�����û����й���.
//V2.6 20111009
//1,������read_addr��write_addr��������.��������������������д�ڲ������ַ(��������Ч��ַ).���ӷ������.
//2,read_addr��write_addr������������ͨ������USMART_USE_WRFUNSΪ��ʹ�ܺ͹ر�.
//3,�޸���usmart_strcmp,ʹ��淶��.			  
//V2.7 20111024
//1,�����˷���ֵ16������ʾʱ�����е�bug.
//2,�����˺����Ƿ��з���ֵ���ж�,���û�з���ֵ,�򲻻���ʾ.�з���ֵʱ����ʾ�䷵��ֵ.
//V2.8 20111116
//1,������list�Ȳ���������ָ��ͺ���ܵ���������bug.
//V2.9 20120917
//1,�޸������磺void*xxx(void)���ͺ�������ʶ���bug��
/////////////////////////////////////////////////////////////////////////////////////
//ϵͳ����
u8 *sys_cmd_tab[]=
{
	"?",
	"help",
	"list",
	"id",
	"hex",
	"dec",	   
};	    
//����ϵͳָ��
//0,�ɹ�����;����,�������;
u8 usmart_sys_cmd_exe(u8 *str)
{
	u8 i;
	u8 sfname[MAX_FNAME_LEN];//��ű��غ�����
	u8 pnum;
	u8 rval;
	u32 res;

	res=usmart_get_cmdname(str,sfname,&i,MAX_FNAME_LEN);//�õ�ָ�ָ���
	if(res)return USMART_FUNCERR;//�����ָ�� 
	str+=i;					 			    
	for(i=0;i<8;i++)//֧��8��ϵͳָ��
	{
		if(usmart_strcmp(sfname,sys_cmd_tab[i])==0)break;
	}
	switch(i)
	{					   
		case 0:
		case 1://����ָ��
			printf("\r\n");
#if USMART_USE_HELP
			printf("------------------------USMART V2.8------------------------ \r\n");
			printf("    USMART����ALIENTEK������һ�����ɵĴ��ڵ��Ի������,ͨ�� \r\n");
			printf("��,�����ͨ���������ֵ��ó���������κκ���,��ִ��.���,���\r\n");
			printf("��������ĺ������������(֧������(10/16����)���ַ�����������\r\n");	  
			printf("�ڵ�ַ����Ϊ����),�����������֧��10���������,��֧�ֺ����� \r\n");
			printf("��ֵ��ʾ.����������ʾ�������ù���,��������ת������.\r\n");
			printf("����֧��:www.openedv.com\r\n");
			printf("USMART��6��ϵͳ����:\r\n");
			printf("?:    ��ȡ������Ϣ\r\n");
			printf("help: ��ȡ������Ϣ\r\n");
			printf("list: ���õĺ����б�\r\n\n");
			printf("id:   ���ú�����ID�б�\r\n\n");
			printf("hex:  ����16������ʾ,����ո�+���ּ�ִ�н���ת��\r\n\n");
			printf("dec:  ����10������ʾ,����ո�+���ּ�ִ�н���ת��\r\n\n");
			printf("�밴�ճ����д��ʽ���뺯�������������Իس�������.\r\n");    
			printf("--------------------------ALIENTEK------------------------- \r\n");
#else
			printf("ָ��ʧЧ\r\n");
#endif
			break;
		case 2://��ѯָ��
			printf("\r\n");
			printf("-------------------------�����嵥--------------------------- \r\n");
			for(i=0;i<usmart_dev.fnum;i++)printf("%s\r\n",usmart_dev.funs[i].name);
			printf("\r\n");
			break;	 
		case 3://��ѯID
			printf("\r\n");
			printf("-------------------------���� ID --------------------------- \r\n");
			for(i=0;i<usmart_dev.fnum;i++)
			{
				usmart_get_fname((u8*)usmart_dev.funs[i].name,sfname,&pnum,&rval);//�õ����غ����� 
				printf("%s id is:\r\n0X%08X\r\n",sfname,usmart_dev.funs[i].func); //��ʾID
			}
			printf("\r\n");
			break;
		case 4://hexָ��
			printf("\r\n");
			usmart_get_aparm(str,sfname,&i);
			if(i==0)//��������
			{
				i=usmart_str2num(sfname,&res);	   	//��¼�ò���	
				if(i==0)						  	//����ת������
				{
					printf("HEX:0X%X\r\n",res);	   	//תΪ16����
				}else if(i!=4)return USMART_PARMERR;//��������.
				else 				   				//������ʾ�趨����
				{
					printf("16���Ʋ�����ʾ!\r\n");
					usmart_dev.sptype=SP_TYPE_HEX;  
				}

			}else return USMART_PARMERR;			//��������.
			printf("\r\n"); 
			break;
		case 5://decָ��
			printf("\r\n");
			usmart_get_aparm(str,sfname,&i);
			if(i==0)//��������
			{
				i=usmart_str2num(sfname,&res);	   	//��¼�ò���	
				if(i==0)						   	//����ת������
				{
					printf("DEC:%lu\r\n",res);	   	//תΪ10����
				}else if(i!=4)return USMART_PARMERR;//��������.
				else 				   				//������ʾ�趨����
				{
					printf("10���Ʋ�����ʾ!\r\n");
					usmart_dev.sptype=SP_TYPE_DEC;  
				}

			}else return USMART_PARMERR;			//��������. 
			printf("\r\n"); 
			break;	 
		case 6://readָ��
			printf("\r\n");
			usmart_get_aparm(str,sfname,&i);
			if(i==0)//��������
			{
				i=usmart_str2num(sfname,&res);	   		//��¼�ò���	
				if(i==0)						   		//��ȡָ����ַ���ݹ���
				{
					if(usmart_dev.sptype==SP_TYPE_HEX)printf("*(0X%X)=0X%X\r\n",res,*(u32*)res);	//��ȡָ����ַ��ֵ
					else printf("*(%d)=%d\r\n",res,*(u32*)res);	//��ȡָ����ַ��ֵ
				}else return USMART_PARMERR;   			//δ������,���߲�������	 
 			}else return USMART_PARMERR;				//��������. 
			printf("\r\n"); 
			break;	    
		default://�Ƿ�ָ��
			return USMART_FUNCERR;
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////
#if USMART_ENTIMX_SCAN==1
//��������������,��USMART����,�ŵ�����,����������ֲ. 
//��ʱ��2�жϷ������	 
void TIM2_IRQHandler(void)
{ 		    		  			    
	if(TIM2->SR&0X0001)//����ж�
	{
		usmart_dev.scan();//ִ��usmartɨ��			    				   				     	    	
	}				   
	TIM2->SR&=~(1<<0);//����жϱ�־λ 	    
}
//ʹ�ܶ�ʱ��2,ʹ���ж�.
void Timer2_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<0;//TIM2ʱ��ʹ��    
 	TIM2->ARR=arr;  //�趨�������Զ���װֵ  
	TIM2->PSC=psc;  //Ԥ��Ƶ��7200,�õ�10Khz�ļ���ʱ��
	//����������Ҫͬʱ���òſ���ʹ���ж�
	TIM2->DIER|=1<<0;   //��������ж�				
	TIM2->DIER|=1<<6;   //�������ж�
		  							    
	TIM2->CR1|=0x01;    //ʹ�ܶ�ʱ��2
  	MY_NVIC_Init(3,3,TIM2_IRQChannel,2);//��ռ3�������ȼ�3����2(��2�����ȼ���͵�)									 
}
#endif
////////////////////////////////////////////////////////////////////////////////////////
//��ʼ�����ڿ�����
//sysclk:ϵͳʱ�ӣ�Mhz��
void usmart_init(u8 sysclk)
{
#if USMART_ENTIMX_SCAN==1
	Timer2_Init(1000,(u32)sysclk*100-1);//��Ƶ,ʱ��Ϊ10K ,100ms�ж�һ��
#endif
	usmart_dev.sptype=1;	//ʮ��������ʾ����
}		
//��str�л�ȡ������,id,��������Ϣ
//*str:�ַ���ָ��.
//����ֵ:0,ʶ��ɹ�;����,�������.
u8 usmart_cmd_rec(u8*str) 
{
	u8 sta,i,rval;//״̬	 
	u8 rpnum,spnum;
	u8 rfname[MAX_FNAME_LEN];//�ݴ�ռ�,���ڴ�Ž��յ��ĺ�����  
	u8 sfname[MAX_FNAME_LEN];//��ű��غ�����
	sta=usmart_get_fname(str,rfname,&rpnum,&rval);//�õ����յ������ݵĺ���������������	  
	if(sta)return sta;//����
	for(i=0;i<usmart_dev.fnum;i++)
	{
		sta=usmart_get_fname((u8*)usmart_dev.funs[i].name,sfname,&spnum,&rval);//�õ����غ���������������
		if(sta)return sta;//���ؽ�������	  
		if(usmart_strcmp(sfname,rfname)==0)//���
		{
			if(spnum>rpnum)return USMART_PARMERR;//��������(���������Դ����������)
			usmart_dev.id=i;//��¼����ID.
			break;//����.
		}	
	}
	if(i==usmart_dev.fnum)return USMART_NOFUNCFIND;	//δ�ҵ�ƥ��ĺ���
 	sta=usmart_get_fparam(str,&i);					//�õ�������������	
	if(sta)return sta;								//���ش���
	usmart_dev.pnum=i;								//����������¼
    return USMART_OK;
}
//usamrtִ�к���
//�ú�����������ִ�дӴ����յ�����Ч����.
//���֧��10�������ĺ���,����Ĳ���֧��Ҳ������ʵ��.�����õĺ���.һ��5�����ҵĲ����ĺ����Ѿ����ټ���.
//�ú������ڴ��ڴ�ӡִ�����.��:"������(����1������2...����N)=����ֵ".����ʽ��ӡ.
//����ִ�еĺ���û�з���ֵ��ʱ��,����ӡ�ķ���ֵ��һ�������������.
void usmart_exe(void)
{
	u8 id,i;
	u32 res;		   
	u32 temp[MAX_PARM];//����ת��,ʹ֧֮�����ַ��� 
	u8 sfname[MAX_FNAME_LEN];//��ű��غ�����
	u8 pnum,rval;
	id=usmart_dev.id;
	if(id>=usmart_dev.fnum)return;//��ִ��.
	usmart_get_fname((u8*)usmart_dev.funs[id].name,sfname,&pnum,&rval);//�õ����غ�����,���������� 
	printf("\r\n%s(",sfname);//�����Ҫִ�еĺ�����
	for(i=0;i<pnum;i++)//�������
	{
		if(usmart_dev.parmtype&(1<<i))//�������ַ���
		{
			printf("%c",'"');			 
			printf("%s",usmart_dev.parm+usmart_get_parmpos(i));
			printf("%c",'"');
			temp[i]=(u32)&(usmart_dev.parm[usmart_get_parmpos(i)]);
		}else						  //����������
		{
			temp[i]=*(u32*)(usmart_dev.parm+usmart_get_parmpos(i));
			if(usmart_dev.sptype==SP_TYPE_DEC)printf("%lu",temp[i]);//10���Ʋ�����ʾ
			else printf("0X%X",temp[i]);//16���Ʋ�����ʾ 	   
		}
		if(i!=pnum-1)printf(",");
	}
	printf(")");
	switch(usmart_dev.pnum)
	{
		case 0://�޲���(void����)											  
			res=(*(u32(*)())usmart_dev.funs[id].func)();
			break;
	    case 1://��1������
			res=(*(u32(*)())usmart_dev.funs[id].func)(temp[0]);
			break;
	    case 2://��2������
			res=(*(u32(*)())usmart_dev.funs[id].func)(temp[0],temp[1]);
			break;
	    case 3://��3������
			res=(*(u32(*)())usmart_dev.funs[id].func)(temp[0],temp[1],temp[2]);
			break;
	    case 4://��4������
			res=(*(u32(*)())usmart_dev.funs[id].func)(temp[0],temp[1],temp[2],temp[3]);
			break;
	    case 5://��5������
			res=(*(u32(*)())usmart_dev.funs[id].func)(temp[0],temp[1],temp[2],temp[3],temp[4]);
			break;
	    case 6://��6������
			res=(*(u32(*)())usmart_dev.funs[id].func)(temp[0],temp[1],temp[2],temp[3],temp[4],\
			temp[5]);
			break;
	    case 7://��7������
			res=(*(u32(*)())usmart_dev.funs[id].func)(temp[0],temp[1],temp[2],temp[3],temp[4],\
			temp[5],temp[6]);
			break;
	    case 8://��8������
			res=(*(u32(*)())usmart_dev.funs[id].func)(temp[0],temp[1],temp[2],temp[3],temp[4],\
			temp[5],temp[6],temp[7]);
			break;
	    case 9://��9������
			res=(*(u32(*)())usmart_dev.funs[id].func)(temp[0],temp[1],temp[2],temp[3],temp[4],\
			temp[5],temp[6],temp[7],temp[8]);
			break;
	    case 10://��10������
			res=(*(u32(*)())usmart_dev.funs[id].func)(temp[0],temp[1],temp[2],temp[3],temp[4],\
			temp[5],temp[6],temp[7],temp[8],temp[9]);
			break;
	}
	if(rval==1)//��Ҫ����ֵ.
	{
		if(usmart_dev.sptype==SP_TYPE_DEC)printf("=%lu;\r\n",res);//���ִ�н��(10���Ʋ�����ʾ)
		else printf("=0X%X;\r\n",res);//���ִ�н��(16���Ʋ�����ʾ)	   
	}else printf(";\r\n");//����Ҫ����ֵ,ֱ���������
	
}
//usmartɨ�躯��
//ͨ�����øú���,ʵ��usmart�ĸ�������.�ú�����Ҫÿ��һ��ʱ�䱻����һ��
//�Լ�ʱִ�дӴ��ڷ������ĸ�������.
//�������������ж��������,�Ӷ�ʵ���Զ�����.
//�����ALIENTEK�û�,��USART_RX_STA��USART_RX_BUF[]��Ҫ�û��Լ�ʵ��
void usmart_scan(void)
{
	u8 sta,len;
	if(USART_RX_STA&0x8000)//���ڽ�����ɣ�
	{					   
		len=USART_RX_STA&0x3fff;	//�õ��˴ν��յ������ݳ���
		USART_RX_BUF[len]='\0';	//��ĩβ���������. 
		sta=usmart_dev.cmd_rec(USART_RX_BUF);//�õ�����������Ϣ
		if(sta==0)usmart_dev.exe();//ִ�к���
		else 
		{  
			len=usmart_sys_cmd_exe(USART_RX_BUF);
			if(len!=USMART_FUNCERR)sta=len;
			if(sta)
			{
				switch(sta)
				{
					case USMART_FUNCERR:
						printf("��������!\r\n");   			
						break;	
					case USMART_PARMERR:
						printf("��������!\r\n");   			
						break;				
					case USMART_PARMOVER:
						printf("����̫��!\r\n");   			
						break;		
					case USMART_NOFUNCFIND:
						printf("δ�ҵ�ƥ��ĺ���!\r\n");   			
						break;		
				}
			}
		}
		USART_RX_STA=0;//״̬�Ĵ������	    
	}
}

#if USMART_USE_WRFUNS==1 	//���ʹ���˶�д����
//��ȡָ����ַ��ֵ		 
u32 read_addr(u32 addr)
{
	return *(u32*)addr;//	
}
//��ָ����ַд��ָ����ֵ		 
void write_addr(u32 addr,u32 val)
{
	*(u32*)addr=val; 	
}
#endif













