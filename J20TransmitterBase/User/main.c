/*
=============J20航模遥控器遥控器端-基础版V1.0==============
	芯片STM32F103C8T6，使用ADC读取电位器的电压采样值，0~4095转换到1000~2000，赋值给PWM输出。
					 TIM2定时触发ADC采样，通过DMA传输给变量ch1Value，取10次进行均值滤波。
	控制舵机的PWM：周期20ms，高电平时间0.5ms~2.5ms变化，可控制舵机0~180°的角度变化，
				   但航模舵面的实际控制中，不可能有180°变化，所以通用的高电平宽度其实是1ms~2ms
	电位器：GND - PA0 - 3.3V
	OLED显示屏：
				GND   电源地
				VCC   接3.3v电源
				SCL   接PB8（SCL）
				SDA   接PB9（SDA）
	串口USB-TTL接法：	
				GND   电源地
				3V3   接3.3v
				TXD   接PB7
				RXD   接PB6
	ST-LINK V2接法：
				GND   电源地
				3V3   接3.3v
				SWCLK 接DCLK
				SWDIO 接DIO
	PWM输出：PB5
	by Bilibili 蔡子CaiZi
*/
#include "adc.h"
#include "delay.h"
#include "usart.h"
#include "stm32f10x.h"
#include "oled.h"
#include "stdio.h"
#include "string.h"
#include "nrf24l01.h"
#include "led.h"
int main()
{
	u16 t=0, tNum=0;
	u8 chPacket[32];
//	u8 txt[16]={0};
	delay_init();//初始化延时函数
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2，2位抢占优先级和2位子优先级
	usart_init(115200);//初始化串口1，波特率为115200
	TIM2_Init(499,71);//1MHz，每500us采集一次；可设置9us以上，但过小影响OLED显示
	DMA1_Init();	//DMA初始化
	GPIOA_Init();	//PA初始化
	Adc_Init();		//ADC初始化
	//RTC_Init();	  	//RTC初始化
	LED_Init();		//LED初始化
	OLED_Init();	//初始化OLED  
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
			LED = !LED;// LED闪烁表示正在发送数据
			tNum = 0;
		}	
//		itoa(PWM1value,txt,10);//将int类型转换成10进制字符串
//		OLED_ShowString(6,3,txt,24); //位置6,3；字符大小24*24点阵
//		OLED_Refresh_Gram();
	}
}
