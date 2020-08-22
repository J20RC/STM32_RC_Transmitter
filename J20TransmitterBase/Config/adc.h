#ifndef __ADC_H
#define __ADC_H
#include "stm32f10x.h" //记得添加此头文件，因为config.c用到GPIO相关函数等
#include "sys.h"
#define adcNum 9		//采样通道数
#define chNum 8	//遥杆通道数
#define sampleNum 10 //每个通道采样次数
#define setDataSize sizeof(setData)/2 //每个通道采样次数

#pragma pack(1)//单字节对齐，很重要！
//不要超过127个16位数据
//计数：1+8*3+8*2+8+1+2
typedef struct _set_Config// 用户设置结构体
{
	u16 writeFlag;//是否第一次写入=2字节16位
	u16 chLower[chNum];//遥杆的最小值=2*chNum
	u16 chMiddle[chNum];//遥杆的中值
	u16 chUpper[chNum];//遥杆的最大值
	int PWMadjustValue[chNum];//微调PWM占空比=4字节*chNum
	u16 chReverse[chNum];//通道的正反，1为正常，0为反转
	u16 PWMadjustUnit;//微调单位
	float warnBatVolt;//报警电压=4个字节32位
}set_Config;
#pragma pack()

extern volatile u16 chValue[adcNum*sampleNum];//ADC采样值
extern volatile u16 chResult[chNum];//滤波后的ADC采样值
extern volatile u16 PWMvalue[chNum];//控制PWM占空比
extern volatile float batVolt;//电池电压
extern volatile u8 batVoltSignal;//是否报警
extern volatile set_Config setData;

void TIM2_Init(u16 arr,u16 psc);//TIM2定时器初始化
void DMA1_Init(void);
void GPIOA_Init(void);
void Adc_Init(void);//ADC1初始化
u16 Get_Adc(u8 ch); //获取一次ADC的值
u16 Get_Adc_Average(u8 ch,u8 times);//ADC采样值进行均值滤波
float map(float value,float fromLow,float fromHigh,float toLow,float toHigh);//映射函数
int mapChValue(int val, int lower, int middle, int upper, int reverse);//通道的重映射函数
int GetMedianNum(volatile u16 * bArray, int ch);//中值滤波
#endif
