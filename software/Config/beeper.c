#include "beeper.h"
#include "delay.h"
	    
//蜂鸣器IO初始化
void BEEPER_Init(void)
{
 
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能PC端口时钟
	
	//初始化 蜂鸣器引脚PA10	  推挽输出
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA10
	GPIO_ResetBits(GPIOA,GPIO_Pin_10);//输出低电平
}
//蜂鸣器短响一次
void beeperOnce(void)
{
	Beeper = 1;
	delay_ms(5);
	Beeper = 0;
}
