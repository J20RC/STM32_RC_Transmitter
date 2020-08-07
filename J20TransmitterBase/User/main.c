/*
=============J20航模遥控器遥控器端-基础版V1.1==============
	开发板：STM32F103C8T6蓝色板
	NRF24L01模块：
				GND   	电源地
				VCC		接3.3V电源
				CSN		接PB12
				SCK		接PB13
				MISO	接PB14
				MOSI	接PB15
				CE		接PA8
				IRQ		接PA9
	ADC采样：PA0-7
	电池电压检测：PB0
	蜂鸣器：PA10
	6个按键：
				CH1Left 接PB5	【home】
				CH1Right接PB4
				CH2Up	接PA15	【返回】
				CH2Down	接PB3	【确定】
				CH4Left	接PA12	【数值-】
				CH4Right接PA11	【数值+】
	旋转编码器模块：
				GND   	电源地
				VCC   	接3.3V电源
				SW		接PB11
				DT		接PB10
				CLK		接PB1
	OLED显示屏：
				GND   	电源地
				VCC   	接3.3V电源
				SCL   	接PB8（SCL）
				SDA   	接PB9（SDA）
	串口USB-TTL接法：	
				GND   	电源地
				3V3   	接3.3V
				TXD   	接PB7
				RXD   	接PB6
	ST-LINK V2接法：
				GND   	电源地
				3V3   	接3.3V
				SWCLK 	接DCLK
				SWDIO 	接DIO
	
	by J20开发团队
*/
#include "adc.h"
#include "delay.h"
#include "usart.h"
#include "rtc.h"
#include "stm32f10x.h"
#include "oled.h"
#include "stdio.h"
#include "string.h"
#include "nrf24l01.h"
#include "led.h"
#include "key.h"
#include "flash.h"
#include "menu.h"
extern unsigned char logo[];
int main()
{
	
	//u8 txt[16]={0};//存放字符串文本
	//u8 sendStatus;
	u16 lastThrPWM = 0;//上一时刻的油门大小
	u16 loca;//存放坐标
	u16 updateWindow[10];//窗口更新标志
	u16 thrNum;//油门换算后的大小
	delay_init();//初始化延时函数
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2，2位抢占优先级和2位子优先级
	
	
	usart_init(115200);//初始化串口1，波特率为115200
	TIM2_Init(1999,71);//1MHz，每10ms进行ADC采样一次
	TIM3_Init(19999,71);//1MHz，每20ms检测按键一次；
	DMA1_Init();	//DMA初始化
	Adc_Init();		//ADC初始化
	RTC_Init();		//RTC初始化
	LED_Init();		//LED初始化
	KEY_Init();		//KEY初始化
	NRF24L01_Init();//初始化NRF24L01
	
	OLED_Init();	//初始化OLED
	OLED_Clear();
	OLED_DrawPointBMP(9,0,logo,110,56,1);//显示logo
	OLED_Refresh_Gram();//刷新显存
	
	while(NRF24L01_Check())
	{
 		delay_ms(200);
		LED = !LED;// LED闪烁表示正在等待无线模块连接
	}
	LED = 0;//熄灭LED
	NRF24L01_TX_Mode();
	delay_ms(1000);
		
	mainWindow();//显示主界面
	OLED_Refresh_Gram();//刷新显存
	while (1){
		if(nowMenuIndex!=0){
			sendDataPacket();//发送数据包
			sendCount++;
		}
		if(sendCount == 5)//每隔10次检查一下
		{
			if(batVoltSignal==1) Beeper = !Beeper;//蜂鸣器间断鸣叫，报警
			else Beeper = 0;//不报警
			LED = !LED;// LED闪烁表示正在发送数据
			if(abs(PWMvalue[2]/20-lastThrPWM)>0) updateWindow[0] = 1;
			lastThrPWM = PWMvalue[2]/20;//将1000量程进行20分频
		}
		
		if(updateWindow[0] && nowMenuIndex==0 && sendCount == 10)//油门更新事件
		{
			thrNum = (int)(PWMvalue[2]-1000)/16;
			OLED_Fill(0,63-thrNum,0,63,1);//下部分写1
			OLED_Fill(0,0,0,63-thrNum,0);//上部分写0
			OLED_Refresh_Gram();//刷新显存
			updateWindow[0] = 0;
			sendCount = 0;
		}
		if(sendCount > 10) sendCount = 0;
		if(keyEvent>0)//微调更新事件
		{
			if(nowMenuIndex==0)
			{
				if(keyEvent==1|keyEvent==2) 
				{
					OLED_Fill(66,59,124,62,0);//写0，清除原来的标志
					loca = (int)95+setData.PWMadjustValue[0]/4;
					OLED_Fill(loca,59,loca,62,1);//写1
					OLED_DrawPlusSign(95,61);//中心标识
				}
				if(keyEvent==3|keyEvent==4) 
				{
					OLED_Fill(123,1,126,63,0);//写0
					loca = (int)32-setData.PWMadjustValue[1]/4;
					OLED_Fill(123,loca,126,loca,1);//写1
					OLED_DrawPlusSign(125,32);//中心标识
				}
				if(keyEvent==5|keyEvent==6) 
				{	
					OLED_Fill(4,59,62,62,0);//写0，清除原来的标志
					loca = (int)33+setData.PWMadjustValue[3]/4;
					OLED_Fill(loca,59,loca,62,1);//写1
					OLED_DrawPlusSign(33,61);//中心标识
				}
				OLED_Refresh_Gram();//刷新显存
			}
			STMFLASH_Write(FLASH_SAVE_ADDR,(u16 *)&setData,setDataSize);//将用户数据写入FLASH
			keyEvent = 0;
		}
		if(nowMenuIndex==13)//通道校准
		{
			subMenu3_1();
			OLED_Refresh_Gram();//刷新显存
			for(int i=0;i<4;i++)
			{
				if(chResult[i]>setData.chUpper[i]) setData.chUpper[i]=chResult[i];
				if(chResult[i]<setData.chLower[i]) setData.chLower[i]=chResult[i];
			}
			STMFLASH_Write(FLASH_SAVE_ADDR,(u16 *)&setData,setDataSize);//将用户数据写入FLASH
		}
		
		if(menuEvent[0])//菜单事件
		{
			OLED_display();
			if(nowMenuIndex==13 && lastMenuIndex != 13)//通道中立点校准
			{
				for(int i=0;i<4;i++)
				{
					setData.chMiddle[i]=chResult[i];
				}
				STMFLASH_Write(FLASH_SAVE_ADDR,(u16 *)&setData,setDataSize);//将用户数据写入FLASH
			}
			
			for(int i=0;i<4;i++)
			{
				if(setData.PWMadjustValue[i]>100-setData.PWMadjustUnit) setData.PWMadjustValue[i]=100-setData.PWMadjustUnit;//限制微调范围
				if(setData.PWMadjustValue[i]<setData.PWMadjustUnit-100) setData.PWMadjustValue[i]=setData.PWMadjustUnit-100;//限制微调范围
			}
			if(menuEvent[1]==NUM_up)
			{
				if(nowMenuIndex==5){setData.PWMadjustValue[0] += setData.PWMadjustUnit;subMenu1_1();}
				if(nowMenuIndex==6){setData.PWMadjustValue[1] += setData.PWMadjustUnit;subMenu1_2();}
				if(nowMenuIndex==7){setData.PWMadjustValue[2] += setData.PWMadjustUnit;subMenu1_3();}
				if(nowMenuIndex==8){setData.PWMadjustValue[3] += setData.PWMadjustUnit;subMenu1_4();}
				if(nowMenuIndex==9) {setData.chReverse[0] = !setData.chReverse[0];subMenu2_1();}
				if(nowMenuIndex==10) {setData.chReverse[1] = !setData.chReverse[1];subMenu2_2();}
				if(nowMenuIndex==11) {setData.chReverse[2] = !setData.chReverse[2];subMenu2_3();}
				if(nowMenuIndex==12) {setData.chReverse[3] = !setData.chReverse[3];subMenu2_4();}
				
			}
			if(menuEvent[1]==NUM_down)
			{
				if(nowMenuIndex==5){setData.PWMadjustValue[0] -= setData.PWMadjustUnit;subMenu1_1();}
				if(nowMenuIndex==6){setData.PWMadjustValue[1] -= setData.PWMadjustUnit;subMenu1_2();}
				if(nowMenuIndex==7){setData.PWMadjustValue[2] -= setData.PWMadjustUnit;subMenu1_3();}
				if(nowMenuIndex==8){setData.PWMadjustValue[3] -= setData.PWMadjustUnit;subMenu1_4();}
				if(nowMenuIndex==9) {setData.chReverse[0] = !setData.chReverse[0];subMenu2_1();}
				if(nowMenuIndex==10) {setData.chReverse[1] = !setData.chReverse[1];subMenu2_2();}
				if(nowMenuIndex==11) {setData.chReverse[2] = !setData.chReverse[2];subMenu2_3();}
				if(nowMenuIndex==12) {setData.chReverse[3] = !setData.chReverse[3];subMenu2_4();}
			}
			STMFLASH_Write(FLASH_SAVE_ADDR,(u16 *)&setData,setDataSize);//将用户数据写入FLASH
			OLED_Refresh_Gram();//刷新显存
			menuEvent[0] = 0;
			menuEvent[1] = BM_NULL;
		}
		lastMenuIndex = nowMenuIndex;
	}
}

