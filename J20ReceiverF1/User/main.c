/*
=============J20航模遥控器接收端-基础版V1.0==============
	芯片STM32F103C8T6，实现了NRF24L01无线接收和8个通道的PWM输出
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
#include "main.h"
#include "delay.h"
#include "usart.h"
#include "stm32f10x.h"
#include "rtc.h" 
#include "stdio.h"
#include "string.h"
#include "nrf24l01.h"
#include "led.h"
#include "tim.h"
#include "sbus.h"
#include "pwm.h"
#include "ppm.h"

u16 PWMvalue[SBUS_CHANNEL_NUMBER];// 控制PWM占空比
u16 recPWMvalue[SBUS_CHANNEL_NUMBER];// 控制PWM占空比
u8 sbusPacket[SBUS_PACKET_LENGTH];// 25个字节的SBUS数据包
u8 signalLoss = 0;  // 1表示丢失信号
u16 i=0,startIndex=0;
u32 lastTime = 0;
u32 sbusTime = 0;
u8 chPacket[32];

void PWM_reset(void);

int main()
{
//	delay_init();//初始化延时函数
	PWM_reset();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2，2位抢占优先级和2位子优先级
	usart_init(100000);//初始化串口1，作为sbus输出，（波特率为100000，8个数据位，偶校验，2个停止位）
	//usart_init(115200);
	TIM3_PWM_Init(19999,71);//预分频72，频率1MHz，周期1us；自动装载值20 000，故PWM周期1us*20 000
	TIM2_PWM_Init(19999,71);//PWM输出
	TIM4_Counter_Init(9,71); //预分频1MHz，周期1us，自动装载值10，故最小计数单位10us
	PPM_Pin_Init();//PPM引脚初始化
	systick_init(10000);//PPM定时初始化，初始值随意设置
	LED_Init();		//LED初始化
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
				recPWMvalue[i] = ((u16)chPacket[startIndex] << 8) | ((u16)(chPacket[startIndex+1]));// 合并u8为u16
				if(recPWMvalue[i]!=0) PWMvalue[i] = recPWMvalue[i];
				startIndex = startIndex+2;
			}
			for (i=chNum; i<16; i++) 
			{
				PWMvalue[i] = 1500;//未用到的通道全部置中
			}
			//printf("%d,%d,%d,%d\n",PWMvalue[4],PWMvalue[5],PWMvalue[6],PWMvalue[7]);
			LED = 0;
			lastTime = nowTime;
		}
		
		if (nowTime > sbusTime) //输出SBUS
		{
			sbusPreparePacket(sbusPacket, PWMvalue, signalLoss, 0); //chNum通道数值转换为SBUS数据包
			for(i=0;i<SBUS_PACKET_LENGTH;i++)
			{
				USART_SendData(USART1,sbusPacket[i]);//将SBUS数据包通过串口TX0输出
				while( USART_GetFlagStatus(USART1,USART_FLAG_TC)!= SET);//等待发送完成
			}
			sbusTime = nowTime + SBUS_UPDATE_RATE*100;//10ms更新一次
		}
//		printf("%d\t%d\n",nowTime,lastTime);
		if(nowTime-lastTime>100*2000)//距离上次接收时间大于2s，则说明失去信号
		{
			LED=!LED;
			signalLoss = 1;//失去信号标志
			PWM_reset();//失控保护
			delay_ms(200);
		}
		//PPM数据转换
		for (i=0;i<chNum;i++) {chTime[i] = PWMvalue[i]*9;}
		TIM_SetCompare1(TIM2,PWMvalue[0]);//输出给PWM-PA0
		TIM_SetCompare2(TIM2,PWMvalue[1]);//输出给PWM-PA1
		TIM_SetCompare3(TIM2,PWMvalue[2]);//输出给PWM-PA2
		TIM_SetCompare4(TIM2,PWMvalue[3]);//输出给PWM-PA3
		TIM_SetCompare1(TIM3,PWMvalue[4]);//输出给PWM-PA6
		TIM_SetCompare2(TIM3,PWMvalue[5]);//输出给PWM-PA7
		TIM_SetCompare3(TIM3,PWMvalue[6]);//输出给PWM-PB0
		TIM_SetCompare4(TIM3,PWMvalue[7]);//输出给PWM-PB1
	}
}
//初始化PWM输出和失控保护
void PWM_reset(void)
{
	PWMvalue[0] = 1500;//通道1置中
	PWMvalue[1] = 1500;//通道2置中
	PWMvalue[2] = 1000;//油门最低
	PWMvalue[3] = 1500;//通道4置中
	for (i=4; i<chNum; i++) 
	{
		PWMvalue[i] = 1500;//未用到的通道全部置中
	}
}

