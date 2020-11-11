#include "ppm.h"

void systick_init(u32 count)
{
	SysTick_Config(count);//此行应放在配置时钟源之前,count赋值给LOAD寄存器
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);//选择外部时钟HCLK,计数9000 000为1s
	SysTick->VAL = 0;//当VAL=0,LOAD寄存器中的重装载值赋值给VAL寄存器作为初值，每经过一个Systick时钟周期,VAL寄存器值-1
}

void SysTick_Handler(void)//中断入口
{
    ppm_output();
}

// PPM-PC13推挽输出
void PPM_Pin_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);  //使能GPIO外设时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;				 //PPM-->PC13 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);					 //根据设定参数初始化GPIOC13
	GPIO_SetBits(GPIOC,GPIO_Pin_13);						 //PC13 输出高
}


u32 chTime[chNum];// 控制PPM占空比

enum PPM_OUTPUT_CH_STATE {
    CH_DOWN_STATE,
    CH_UP_STATE,
};

static enum PPM_OUTPUT_CH_STATE state = CH_DOWN_STATE;

#define MS20	(9000 * 20)	/* 20ms */
#define MS05	(4500)		/* 0.5ms */

static uint64_t total_value = 0;
static u8 ch_idx = 0;

void ppm_output(void)
{
    u32 ch_val = 0;
    
    /* CH1 ~ CH8和最后一个低电平时间段 */
    if (state == CH_DOWN_STATE) {
		/* 0.5ms后进入下一个定时中断 */
        systick_init(MS05);
        total_value += MS05;
        state = CH_UP_STATE;
        PPM_Pin = 0;
    } 
	else {
        /* CH1 ~ CH8 高电平*/
        if (ch_idx < chNum) {
            if (chTime[ch_idx] < (MS05))
                ch_val = MS05;//使两个上升沿间隔在1ms以上
            else {
                ch_val = chTime[ch_idx] - MS05;
            }
            systick_init(ch_val);
            total_value += ch_val;
            ch_idx++;
        } 
		else {
            /* 最后一个高电平 */
            systick_init(MS20 - total_value);
            total_value = 0;
            ch_idx = 0;
        }
        state = CH_DOWN_STATE;
        PPM_Pin = 1;
    }     
}

