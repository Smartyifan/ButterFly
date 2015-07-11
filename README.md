# Butter Fly  

* * *


### 贾一帆、邓卓 、陈灿思   
卓俊之光

### 个人主页：  
#### 贾一帆：[LOFTER ]( http://nottechnology.lofter.com/)   



* * *  

##目录  
###  [项目说明](#项目说明)  
###  [引脚说明](#引脚说明)  
### [功能说明](#功能说明)  
### [代码版本分支](#代码版本分支)  
### [过程记录](#过程记录)  

* * *
  
## <a name = "项目说明" />项目说明  
**2015/7/4 19:46:06**   
	基于STM32F1系列微控制器的四旋翼飞行控制器，用于挑战无人机自动控制。  
* * *

## <a name = "引脚说明" />引脚说明  
### 三色LED  
| RLED | GLED  | BLED  |  
|------|-------|-------|  
| PE.8 | PE.10 | PE.12 |  
### HC05    
| USART1Tx | USART1Rx |  Key  |  LED  |  
|----------|----------|-------|-------|  
|   PA.9   |   PA.10  | PD.1  |  PD.0 |  
### KS103
| SDA  | SCL |  
|------|-----|  
| PF.0 | PF.1|  

### JY-901
| USART2Tx | USART2Rx |  
|----------|----------|  
|   PA.2   |   PA.3  |
* * *

## <a name = "功能说明" />功能说明  
### 目录  
[HC-05](#HC-05)  
[KS103](#KS103)  
[JY-901](#JY901)  
- - -  
### <a name = "HC-05" /> HC-05  
**2015/7/4 22:14:12**    
	HC-05蓝牙串口模块，用于微控制器与上位机通讯。   
**2015-07-06 16:26:56**  
	使用DMA作为发送与接收通道，提高CPU效率  

_ _ _  

### <a name = "KS103" />KS103  
**July 10, 2015 2:54 PM**  
	IIC通讯的超声波测距模块，测量飞机离地面的距离。  
	超声波模块测量角度达到70°，附近不可有障碍物体，否则会出现极大误差。  

_ _ _

### <a name = "JY901" />JY-901  
**July 11, 2015 6:05 PM**
	使用串口通讯的九轴姿态传感器模块，可使用上位机调试参数。
    使用串口2的DMA接收数据，模块发送频率初步定为20Hz。


* * *

## <a name = "代码版本分支"/>代码版本分支  
### 分支目录  
####  主线分支  
#### [master](#master)   
#### [dev](#dev)  

#### feature分支  
##### [HC-05](#HC-05branch)
##### [KS103](#KS103branch)
##### [JY-901](#JY901branch)
* * *

### <a name = "master" />master分支  
#### 分支说明 
	发布稳定版本的分支  
#### 版本号目录  
[0234ef1](#0234ef1master)  
[d82a586](#d82a586master)  

#### <a name = "0234ef1master" />0234ef1  
	新建空白工程  
#### <a name = "d82a586master" />d82a586  
	初始化git 版本库  
    
- - -

### <a name = "dev" /> dev分支  
#### 分支说明  
	发布待稳定版本的分支  
#### 版本号目录  
[e5414c6](#e5414c6)  
[2f58f1d](#2f58f1d)  
[3f36b67](#3f36b67)  
[0234ef1](#0234ef1)  
[d82a586](#d82a586)  

_ _ _  
#### <a name = "e5414c6" />e5414c6-- Changed Success  
**July 9, 2015 18：00 PM  **  
	更换程序到飞控板
#### <a name = "2f58f1d" />2f58f1d--fix a bug of hc-05.c  
**July 7, 2015 12:00 PM**  
	修正了关于HC05第一次发送，DMA通道值初始化不为0的BUG。
#### <a name = "3f36b67" />3f36b67--Merge branch 'HC05' into dev  
**July 6, 2015 8:45 PM**  
	合并HC-05外设程序  
#### <a name = "0234ef1" />0234ef1-- a blank project.  
	新建空白工程  
#### <a name = "d82a586" />d82a586--git init.  
	初始化git 版本库  
_________________________________________
### <a name = "HC-05branch" />  HC-05  
##### 分支说明 (该分支已删除)
**2015-07-06 16:31:07**  
	HC-05蓝牙串口 通讯模块  
	可用串口：USART1~USART3  
	关键文件：`hc05.c`，`hc05.h`  
    文件目录：`Hardware/HC05`
	使用方法：    
1. 在`hc05.h`的`Define`中，定义是否使用DMA发送、DMA接收（默认使能）    
2. 在`hc05.h`的`Define`中，定义发送和接收数组长度  
3. 在`hc05.h`的`Define`中，定义Key和LED引脚  
4. 在程序开始前，对结构体参数如下赋值： 
``` C  
	/* HC05 -------------------------------------------------------*/  
	HC05.USARTBASE = USART2;		//使用串口2	  
	HC05.KeyBase = GPIOC_BASE;		//Key引脚  
	HC05.KeyPin = GPIO_Pin_7;  

	HC05.LEDBase = GPIOC_BASE;		//LED引脚  
	HC05.LEDPin = GPIO_Pin_8;   
```   
5. 调用`HC05Init(&HC05)`函数，若检测到HC-05模块，返回值为ErrorStatus类型的SUCCESS  
6. 检测成功后，可与配对的蓝牙模块互发数据
7. 发送方法一：使用`HC05printf`函数（此函数用法与printf函数相同）
``` c  
    void HC05printf(HC05Str * HC05,DMA_Channel_TypeDef*DMA_CHx,char* fmt,...)  
    例： HC05printf(&HC05,DMA1_Channel7,"print a float %f\r\n",102.324);      
    //函数内已包含检测上一次传输是否完成的函数  
```
8. 方法二：使用`UARTxDMASend`函数  
	写好`HC05.TxData`数组内容，然后将需要发送的字节数与DMA通道号（USART2为通道7）作为参数开启一次		DMA传输  
``` c  
    UARTxDMASend(DMA_Channel_TypeDef * DMA_CHx,u16 Len);  
    例：for(i = 0;i<HC05.RxLen;i++)HC05.TxData[i] = HC05.RxData[i];  
    while(DMA1_Channel7->CNDTR!=0);	    //检查上一次传输是否完成  
    UARTxDMASend(DMA1_Channel7,HC05.RxLen);  
```
9. 接收使用USART空闲中断，中断中得到接收到的数据包长度，并且进行处理，详情见`hc05.c`中串口2的中断服		务函数    	
_ _ _  
  
### <a name = "KS103branch" /> KS103  
##### 分支说明 (该分支已删除)  
**July 10, 2015 2:59 PM**  
	KS103超声波模块，使用IIC协议与单片机通讯。
	关键文件：`ks103.c`、`ks103.h`、`ks103iic.c`、`ks103iic.h`
    文件目录：`Hardware/KS103`
    使用方法：
1.在`ks103iic.h`中设置相关引脚的宏定义。
2.在初始化中设置`SDA`、`SCL`的引脚与设备地址。
``` c  
    KS103.I2C.ClkGPIOBase = GPIOF_BASE;
	KS103.I2C.ClkPin = GPIO_Pin_1;
	
	KS103.I2C.SDAGPIOBase = GPIOF_BASE;
	KS103.I2C.SDAPin = GPIO_Pin_0;
	
	KS103.SlaveAddress = SlaveAddress1;		//设备地址
```
3.编写SCL进入外部中断`SCLInEXTI`函数与外部中断服务函数`EXTIx_IRQHandler`。  
4.调用`Read_KS10X_Data`执行一次探测，打开外部中断，参数`range`为量程与精度指令。
5.执行探测指令后，至少要等待几十ms，为了减少等待的无用时间，使用SCL线的外部中断置位探测完成标志并读取	数据。只有`KS103.detected == SUCCESS`,才能读取`KS103.Height`。
6.外部中断抢占优先级0，子优先级0。


_ _ _

### <a name = "JY901branch" />  JY-901  
##### 分支说明 (该分支已删除)
**July 11, 2015 6:07 PM**
	九轴姿态传感器模块  串口通讯  
	可用串口：USART1~USART3  
	关键文件：`jy901.c`，`jy901.h`  
    文件目录：`Hardware/JY901`
	使用方法：    
1.在jy901.h中定义接收帧个数（模块发送数据个数在上位机中设置，初步设置为发送时间、加速度、角速度、角度、高度这五个数据）。
2.在初始化中设置模块通讯所用串口。
3.编写甄别包头的UART2中断函数（只需修改FH数组中第二个数据的值即可）。包头为第一个数据的帧头，若根据1中设置，包头即为0x55，0x50。
4.编写DMA发送完成中断服务函数，修改memcpy函数中的参数，初步定为在此函数内对飞机进行PID调节。
5.根据需要修改串口、DMA中断的优先级，在.c文件的最下方xxNVICInit函数中。


* * *

## <a name = "过程记录" /> 过程记录  
**2015-07-04 22:42:10**   
	HC-05蓝牙串口模块，使用绑定地址模式，并设置相应地址，可达到一一对应连接的效果。   
**2015-07-06 16:16:23**   
	使用DMA发送 前，要先用    
``` c  
	while(DMA1_Channel7->CNDTR!=0);	
```  
	判断DMA是否完成上一次传输  
**July 7, 2015 5:01 PM**  
	修复一个BUG，CNDTR寄存器复位值为0x0032，该BUG使第一次执行HC05Printf时陷入死循环。    
	在DMAInit()函数后用软件将该寄存器的值设置为0。  
**July 9, 2015 5:54 PM**  
	与KS103通过IIC通讯，需要接上拉电阻  
**July 11, 2015 6:16 PM**     
	完成JY-901模块的程序编写。
    随着中断使用个数的增加，优先级的设置成了一个大麻烦。在以后考虑使用嵌入式系统进行改进。