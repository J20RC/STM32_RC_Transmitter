#ifndef __CONFIG_H
#define __CONFIG_H
#include "stm32f10x.h" //记得添加此头文件，因为config.c用到GPIO相关函数等
#include "sys.h"
extern volatile u16 ch1Value[10];//ADC采样值
extern volatile u16 PWM1value;//控制PWM占空比
void TIM2_Init(u16 arr,u16 psc);//TIM2定时器初始化
void TIM3_PWM_Init(u16 arr,u16 psc);//PB5定时器初始化
void DMA1_Init(void);
void GPIOA_Init(void);
void Adc_Init(void);//ADC1初始化
u16 Get_Adc(u8 ch); //获取一次ADC的值
u16 Get_Adc_Average(u8 ch,u8 times);//ADC采样值进行均值滤波
float map(float value,float fromLow,float fromHigh,float toLow,float toHigh);//映射函数
int GetMedianNum(volatile u16 * bArray, int iFilterLen);//中值滤波
#endif
