#include "tim.h" //记得添加此头文件，因为config.c用到GPIO相关函数等
#include "sys.h"
#include "pwm.h"

#define MS20 ((int)(500*20/1.15))	/* 20ms */
#define MS05 ((int)(250/1.15))		/* 0.5ms */
enum PPM_OUTPUT_CH_STATE {
    CH_DOWN_STATE,
    CH_UP_STATE,
};

static enum PPM_OUTPUT_CH_STATE state = CH_DOWN_STATE;
static uint64_t total_value = 0;
static uint8_t ch_idx = 0;
uint32_t ch_val = 0;
volatile u32 nowTime = 0;//程序运行时间，单位10us
u32 ppmCount = 0;//ppm计数，2us一次
volatile u32 chTime[chNum] = {0};//72000--1ms,72000*2--2ms


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
		ppmCount++;
		ppm_output();
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


void ppm_output(void){
    /* 对于CH1~CH8和最后一个低电平时间段*/
    if(ppmCount>=total_value)
	{
		if(total_value==MS20)
		{
			total_value = 0;
			ppmCount = 0; 
		}
		if (CH_DOWN_STATE == state) {
			/* 0.5ms后产生下一个systick中断 */
			total_value += MS05;
			state = CH_UP_STATE;
			PPM_Pin = 0;
		} 
		else {
			/* 通道CH1~CH8高电平时间段 */
			if (ch_idx < chNum) {
				if (chTime[ch_idx] < (MS05))
					ch_val = MS05;
				else {
					ch_val = (chTime[ch_idx]) - MS05;
				}
				total_value += ch_val;
				ch_idx++;
			} 
			else {
				/* 最后一个长的高电平间隔 */
				total_value = MS20;
				ch_idx = 0;
			}
			state = CH_DOWN_STATE;
			PPM_Pin = 1;
		}
	}
}


