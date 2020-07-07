#ifndef __PWM_H
#define __PWM_H
#include "stm32f10x.h" //记得添加此头文件，因为config.c用到GPIO相关函数等
#include "sys.h"
#define chNum 10		//采样通道数
//extern u16 PWMvalue[chNum];//控制PWM占空比

void TIM3_PB5_PWM_Init(u16 arr,u16 psc);//PB5定时器初始化
void TIM2_PWM_Init(u16 arr,u16 psc);//PA0-3PWM输出初始化
void TIM3_PWM_Init(u16 arr,u16 psc);//PB5定时器初始化
#endif
