#include "setting.h"
#include "main.h"

volatile set_Config setData;
volatile u32 setDataSize=sizeof(setData)/2;
//恢复默认设置
void resetData(void)
{
	for(int i=0;i<chNum;i++)
	{
		setData.chLower[i] 	= 0;	//遥杆的最小值
		setData.chMiddle[i] = 2047;	//遥杆的中值
		setData.chUpper[i] 	= 4095;	//遥杆的最大值
		setData.PWMadjustValue[i]=0;//微调值
		setData.chReverse[i] = 1;	//通道的正反，1为正常，0为反转
	}
	setData.PWMadjustUnit = 2;//微调单位
	setData.warnBatVolt = 3.7;//报警电压
	setData.throttlePreference = 1;//左手油门
	setData.batVoltAdjust = 1000;//电池电压校准值
	setData.modelType = 0;//模型类型：翼0，车1，船2
}
void set_Init(void)
{
	STMFLASH_Read(FLASH_SAVE_ADDR,(u16 *)&setData,setDataSize);//从FLASH中读取结构体
	if(setData.writeFlag!=0x1111){
		setData.writeFlag=0x1111;//是否第一次写入
		resetData();
		STMFLASH_Write(FLASH_SAVE_ADDR,(u16 *)&setData,setDataSize);//写入FLASH
	}
}

