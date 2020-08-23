#ifndef __SBUS_H
#define __SBUS_H
#include "stm32f10x.h" //记得添加此头文件，因为config.c用到GPIO相关函数等
#include "sys.h"
#define RC_CHANNEL_MIN 1000 	//每个通道的PWM最小值
#define RC_CHANNEL_MAX 2000 	//每个通道的PWM最大值
#define SBUS_MIN_OFFSET 173 	//SBUS协议的最小值
#define SBUS_MID_OFFSET 992 	//SBUS协议的中间值
#define SBUS_MAX_OFFSET 1811 	//SBUS协议的最大值
#define SBUS_CHANNEL_NUMBER 16 	//SBUS通道数，16个模拟通道，2个数字通道
#define SBUS_PACKET_LENGTH 25	// SBUS数据包的字节长度
#define SBUS_FRAME_HEADER 0x0f		// SBUS数据头，开始字节
#define SBUS_FRAME_FOOTER 0x00		// SBUS数据尾，结束字节
#define SBUS_FRAME_FOOTER_V2 0x04	// SBUS数据尾，结束字节2
#define SBUS_STATE_FAILSAFE 0x08	// 激活故障安全标志
#define SBUS_STATE_SIGNALLOSS 0x04 	// 丢失信号标志
#define SBUS_UPDATE_RATE 10 //SBUS输出更新时间ms

void sbusPreparePacket(u8 packet[], u16 channels[], u8 isSignalLoss, u8 isFailsafe);
void sbusData(u8 SBUS_DATA[], u16 CH_Rec[]);
float map(float value,float fromLow,float fromHigh,float toLow,float toHigh);

#endif

