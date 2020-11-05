#ifndef __SETTING_H
#define __SETTING_H			  	 
#include "sys.h"
#include "flash.h"
#include "adc.h"

#pragma pack(1)//单字节对齐，很重要！
//不要超过127个16位数
//计数：1+8*3+8*2+8+1+2+1+1
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
	u16 throttlePreference;//左右手油门，1为左手油门
	u16 batVoltAdjust;//电池电压校准值
	u16 modelType;//模型类型
}set_Config;
#pragma pack()


extern volatile set_Config setData;
extern volatile u32 setDataSize;
void set_Init(void);
void resetData(void);

#endif
