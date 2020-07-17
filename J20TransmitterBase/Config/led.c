#include "led.h"
	    
//LED 蜂鸣器IO初始化
void LED_Init(void)
{
 
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC, ENABLE);	 //使能PC端口时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;				 //LED0-->PB.5 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);					 //根据设定参数初始化GPIOC13
	
	//初始化 蜂鸣器引脚PA10	  推挽输出
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10;	
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA10
	GPIO_ResetBits(GPIOA,GPIO_Pin_10);						 //PA10 输出低
	GPIO_SetBits(GPIOC,GPIO_Pin_13);						 //PC13 输出高

}
 
