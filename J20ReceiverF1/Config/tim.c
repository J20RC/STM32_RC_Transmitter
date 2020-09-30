#include "main.h"
#include "tim.h"
#include "sys.h"
#include "pwm.h"

volatile u32 nowTime = 0;//程序运行时间，单位10us

void TIM4_Counter_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE); 
	
    TIM_TimeBaseInitStructure.TIM_Period = arr; //自动重装载寄存器周期的值
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //预分频值
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; // 向上计数
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; //时钟分割为0,仍然使用72MHz
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);//允许更新中断
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);
	
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
	TIM_Cmd(TIM4,ENABLE);
}

void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4, TIM_IT_Update)!=RESET)//检查TIM4更新中断发生与否
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);//清除TIM4更新中断标志
		nowTime++;
	}
}

void Clock_Start(){
	TIM4->CNT=0x00;
	TIM_Cmd(TIM4,ENABLE);
}

u32 Clock_End(){
	u32 result;
	result = TIM4->CNT;
	TIM_Cmd(TIM4,DISABLE);
	return result;
}

