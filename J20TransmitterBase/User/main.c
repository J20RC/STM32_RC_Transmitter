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
				CH1Left 接PB5
				CH1Right接PB4
				CH2Up	接PA15
				CH2Down	接PB3
				CH4Left	接PA12
				CH4Right接PA11
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

int main()
{
	u16 t=0, tNum=0;
	u8 chPacket[32];
	for(int i=0;i<chNum;i++)
	{
		chLower[i] 	= 0;	//遥杆的最小值
		chMiddle[i] = 2047;	//遥杆的中值
		chUpper[i] 	= 4095;	//遥杆的最大值
		chReverse[i] = 1;	//通道的正反，1为正常，0为反转
	}
	delay_init();//初始化延时函数
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2，2位抢占优先级和2位子优先级
	usart_init(115200);//初始化串口1，波特率为115200
	TIM2_Init(499,71);//1MHz，每500us进行ADC采样一次；
	TIM3_Init(19999,71);//1MHz，每20ms检测按键一次；
	DMA1_Init();	//DMA初始化
	Adc_Init();		//ADC初始化
	RTC_Init();		//RTC初始化
	LED_Init();		//LED初始化
	KEY_Init();		//KEY初始化
	OLED_Init();	//初始化OLED,这一步仿真耗时6s
	OLED_Clear();
	NRF24L01_Init();//初始化NRF24L01
	while(NRF24L01_Check())
	{
 		delay_ms(200);
	}
	NRF24L01_TX_Mode();
	while (1){
		
		if(NRF24L01_TxPacket(chPacket)==TX_OK)
		{
			//printf("%d\n",PWMvalue[1]);
			for(t=0;t<16;t++)
			{
				if(t==0)//加入数据头00
				{
					chPacket[2*t]=0x00;
					chPacket[2*t+1]=0x00;
				}
				else if(t<=chNum)
				{
					chPacket[2*t] = (u8)(PWMvalue[t-1]>>8)&0xFF; //高8位，把u16拆分成两个u8进行传输
					chPacket[2*t+1] = (u8)PWMvalue[t-1]&0xFF; //低8位
				}
				else 
				{
					chPacket[2*t]=0xff;
					chPacket[2*t+1]=0xff;
				}
			} 
			tNum ++;
		}
		if(tNum > 500)
		{
			if(batVoltSignal==1) Beeper = !Beeper;//蜂鸣器间断鸣叫，报警
			else Beeper = 0;//不报警
			LED = !LED;// LED闪烁表示正在发送数据
			tNum = 0;
		}	
//		itoa(PWM1value,txt,10);//将int类型转换成10进制字符串
//		OLED_ShowString(6,3,txt,24); //位置6,3；字符大小24*24点阵
//		OLED_Refresh_Gram();
	}
}

