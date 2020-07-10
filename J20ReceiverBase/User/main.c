/*
=============J20航模遥控器接收端-基础版V1.0==============
	芯片STM32F103C8T6，实现了NRF24L01无线接收和10个通道的PWM输出
	NRF24L01模块：
				GND   	电源地
				VCC		接3.3V电源
				CSN		PB12
				SCK		PB13
				MISO	PB14
				MOSI	PB15
				CE		PA8
				IRQ		PA9
	PWM输出：PA0-3，PA6-7，PB0-1
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
#include "pwm.h"
#include "delay.h"
#include "usart.h"
#include "stm32f10x.h"
#include "oled.h"
#include "rtc.h" 
#include "stdio.h"
#include "string.h"
#include "nrf24l01.h"
#include "led.h"
#include "tim.h"

u16 PWMvalue[chNum];//控制PWM占空比
int main()
{
	u16 i=0,startIndex=0;
	u32 lastTime = 0;
	u8 chPacket[32];
//	u8 txt[16]={0};
	delay_init();//初始化延时函数
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2，2位抢占优先级和2位子优先级
	usart_init(115200);//初始化串口1，波特率为115200
	TIM3_PWM_Init(19999,71);//预分频72，频率1MHz，周期1us；自动装载值20 000，故PWM周期1us*20 000
	TIM2_PWM_Init(19999,71);
	TIM4_Counter_Init(9,71); //预分频1MHz，周期1us，自动装载值10，故最小计数单位10us
	RTC_Init();	  	//RTC初始化
	LED_Init();		//LED初始化
	OLED_Init();	//初始化OLED  
	OLED_Clear();
	NRF24L01_Init();//初始化NRF24L01
	while(NRF24L01_Check())
	{
 		delay_ms(200);
	}
		
	NRF24L01_RX_Mode();
	while (1){ 	
		if(NRF24L01_RxPacket(chPacket)==0)
		{
			for(i=0;i<32-chNum*2;i++)//寻找起始位置，一般在第2-3个元素
			{
				if(chPacket[i]==0x00 & chPacket[i+1]==0x00) 
				{
					startIndex = i+2;
					break;
				}
			}
			for(i=0;i<chNum;i++)//从起始位置开始处理
			{
				PWMvalue[i] = ((u16)chPacket[startIndex] << 8) | ((u16)(chPacket[startIndex+1]));// 合并u8为u16
				startIndex = startIndex+2;
			}
			TIM_SetCompare1(TIM2,PWMvalue[0]);//输出给PWM-PA0
			TIM_SetCompare2(TIM2,PWMvalue[1]);//输出给PWM-PA1
			TIM_SetCompare3(TIM2,PWMvalue[2]);//输出给PWM-PA2
			TIM_SetCompare4(TIM2,PWMvalue[3]);//输出给PWM-PA3
			TIM_SetCompare1(TIM3,PWMvalue[4]);//输出给PWM-PA6
			TIM_SetCompare2(TIM3,PWMvalue[5]);//输出给PWM-PA7
			TIM_SetCompare3(TIM3,PWMvalue[6]);//输出给PWM-PB0
			TIM_SetCompare4(TIM3,PWMvalue[7]);//输出给PWM-PB1
			//printf("%d\t%d\t%d\t%d\t%d\n",PWMvalue[0],PWMvalue[1],PWMvalue[2],PWMvalue[3],PWMvalue[4]);
			LED = 0;
			lastTime = nowTime;
//			if(PWMvalue[1]>1800) testPin = 1;
//			else testPin = 0;
		}
//		printf("%d\t%d\n",nowTime,lastTime);
		if(nowTime-lastTime>100*2000)//距离上次接收时间大于2s，则说明失去信号
		{
			LED=!LED;
			delay_ms(200);
		}
//		itoa(PWM1value,txt,10);//将int类型转换成10进制字符串
//		OLED_ShowString(6,3,txt,24); //位置6,3；字符大小24*24点阵
//		OLED_Refresh_Gram();
	}
}
