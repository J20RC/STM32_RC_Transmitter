/*
J20航模遥控器遥控器端
	微控制器：STM32F103C8T6
	版本：基础版V2.2
	具体说明见Doc/ReadMe.txt
	仓库：https://github.com/J20RC/STM32_RC_Transmitter
*/
#include "main.h"

void keyEventHandle(void);
void menuEventHandle(void);
	
extern unsigned char logo[];
u16 lastThrPWM = 0;//上一时刻的油门大小
u16 loca;//存放坐标
u16 updateFlag;//窗口更新标志
u16 thrNum;//油门换算后的大小
float lastBatVolt=0.00;//上一时刻的电池电压
extern char batVoltStr[8];//电池电压字符串
extern char timeStr[9];//时间字符串
u16 count=0;
int main()
{
	delay_init();//初始化延时函数
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2，2位抢占优先级和2位子优先级
	usart_init(115200);//初始化串口1，波特率为115200
	TIM2_Init(1999,71);//1MHz，每10ms进行ADC采样一次
	TIM3_Init(19999,71);//1MHz，每20ms检测按键一次；
	TIM4_Counter_Init(999,7199);//每0.1s计数一次，系统计时
	DMA1_Init();	//DMA初始化
	Adc_Init();		//ADC初始化
	set_Init();	//读取用户数据
	RTC_Init();		//RTC初始化
	BEEPER_Init();	//BEEPER初始化
	KEY_Init();		//KEY初始化
	NRF24L01_Init();//NRF24L01初始化
//	PPM_Pin_Init();//PPM初始化
//	systick_init(10000);//PPM定时初始化，初始值随意设置
	
	OLED_Init();	//初始化OLED
	OLED_Clear();
	OLED_DrawPointBMP(9,0,logo,110,56,1);//显示logo
	OLED_Refresh_Gram();//刷新显存
	onSound();
	while(NRF24L01_Check())
	{
 		delay_ms(100);
		Beeper = !Beeper;//蜂鸣器1Hz报警，表示无线模块故障
	}
	if(setData.NRF_Mode == ON)  NRF24L01_TX_Mode();//发射模式
	else NRF24L01_LowPower_Mode();//掉电模式
	
	delay_ms(1000);
	OLED_Fill(0,0,127,63,0);//清空
	mainWindow();//显示主界面
	OLED_Refresh_Gram();//刷新显存
	while (1){
		if(count%500==0 && nowMenuIndex==home)//检测电池电压
		{
			if(batVoltSignal==1) onSound();//蜂鸣器间断鸣叫，报警
			sprintf((char *)batVoltStr,"%1.2fV",batVolt);
			OLED_ShowString(80,19, (u8 *)batVoltStr,16,1);//显示电池电压
			OLED_Refresh_Gram();//刷新显存
		}
		if(count%100==0 && nowMenuIndex==home)//显示时间
		{
			drawClockTime();//显示时间00:00:00
			showSwState();//显示后四个通道状态
			OLED_Refresh_Gram();//刷新显存
		}
		if(count%20 == 0 && nowMenuIndex==home)//油门更新事件
		{
			if(abs(PWMvalue[2]-lastThrPWM)/10>0) updateFlag = 1;
			lastThrPWM = PWMvalue[2];//1000量程=100%
			if(updateFlag)
			{
				thrNum = (int)(PWMvalue[2]-1000)/22;//更新油门
				if(setData.throttlePreference)//左手油门
				{
					OLED_Fill(2,62-thrNum,2,62,0);//下部分写1
					OLED_Fill(2,16,2,62-thrNum,1);//上部分写0
				}
				else{//右手油门
					OLED_Fill(125,62-thrNum,125,62,0);//下部分写1
					OLED_Fill(125,16,125,62-thrNum,1);//上部分写0
				}
				OLED_Refresh_Gram();//刷新显存
				updateFlag = 0;
			}
		}
		if(keyEvent>0)//微调更新事件
		{
			keyDownSound();
			keyEventHandle();
		}
		if(nowMenuIndex==xcjz14)//行程校准
		{
			menu_xcjz14();
			OLED_Refresh_Gram();//刷新显存
			for(int i=0;i<4;i++)
			{
				if(chResult[i]>setData.chUpper[i]) setData.chUpper[i]=chResult[i];
				if(chResult[i]<setData.chLower[i]) setData.chLower[i]=chResult[i];
			}
		}
		if(nowMenuIndex==dljs18)//舵量监视
		{
			menu_dljs18();
			OLED_Refresh_Gram();//刷新显存
		}
		if(menuEvent[0])//菜单事件
		{
			keyDownSound();
			menuEventHandle();
		}
		lastMenuIndex = nowMenuIndex;
		count++;
	}
}


//微调事件处理函数：更新主界面
void keyEventHandle(void)
{
	if(nowMenuIndex==home)
	{
		if(keyEvent==1|keyEvent==2) 
		{
			if(setData.throttlePreference)//左手油门
			{//第1通道微调-右横线
				OLED_Fill(66,61,119,61,1);//写1，清除原来的标志
				loca = (int)93+setData.PWMadjustValue[0]/12.5;
				OLED_Fill(loca-2,61,loca+2,61,0);//写0
			}
			else//右手油门
			{//第4通道微调-右横线
				OLED_Fill(66,61,119,61,1);//写1，清除原来的标志
				loca = (int)93+setData.PWMadjustValue[3]/12.5;
				OLED_Fill(loca-2,61,loca+2,61,0);//写0
			}
		}
		if(keyEvent==3|keyEvent==4) 
		{
			if(setData.throttlePreference)//左手油门
			{//第2通道微调-右竖线
				OLED_Fill(125,16,125,62,1);//写1
				loca = (int)39-setData.PWMadjustValue[1]/14.29;
				OLED_Fill(125,loca-2,125,loca+2,0);//写1
			}
			else//右手油门
			{//第2通道微调-左竖线
				OLED_Fill(2,16,2,56,1);//写1
				loca = (int)39-setData.PWMadjustValue[1]/12;
				OLED_Fill(2,loca-2,2,loca+2,0);//写1
			}
		}
		if(keyEvent==5|keyEvent==6) 
		{	
			if(setData.throttlePreference)//左手油门
			{//第4通道微调-左横线
				OLED_Fill(7,61,61,61,1);//写1，清除原来的标志
				loca = (int)35+setData.PWMadjustValue[3]/12.5;
				OLED_Fill(loca-2,61,loca+2,61,0);//写0
			}
			else//右手油门
			{//第1通道微调-左横线
				OLED_Fill(7,61,61,61,1);//写1，清除原来的标志
				loca = (int)34+setData.PWMadjustValue[0]/12.5;
				OLED_Fill(loca-2,61,loca+2,61,0);//写0
			}
		}
		OLED_Refresh_Gram();//刷新显存
		STMFLASH_Write(FLASH_SAVE_ADDR,(u16 *)&setData,setDataSize);//将用户数据写入FLASH
	}
	keyEvent = 0;
}

//菜单事件处理函数：更新菜单界面、数据处理
void menuEventHandle(void)
{
	OLED_display();
	if(nowMenuIndex==xcjz14 && lastMenuIndex != xcjz14)//通道中立点校准
	{
		for(int i=0;i<4;i++)
		{
			setData.chLower[i] 	= chResult[i];	//遥杆的最小值更新
			setData.chMiddle[i] = chResult[i];	//遥杆的中值
			setData.chUpper[i] 	= chResult[i];	//遥杆的最大值更新
		}
	}
	
	for(int i=0;i<4;i++)//限制微调范围
	{
		if(setData.PWMadjustValue[i]>300-setData.PWMadjustUnit) setData.PWMadjustValue[i]=300-setData.PWMadjustUnit;
		if(setData.PWMadjustValue[i]<setData.PWMadjustUnit-300) setData.PWMadjustValue[i]=setData.PWMadjustUnit-300;
	}
	if(setData.PWMadjustUnit>8) setData.PWMadjustUnit = 8;//限制微调单位范围
	if(setData.PWMadjustUnit<1) setData.PWMadjustUnit = 2;
	if(menuEvent[1]==NUM_up)
	{
		if(nowMenuIndex==tdwt1){setData.PWMadjustValue[0] += setData.PWMadjustUnit;menu_tdwt1();}
		if(nowMenuIndex==tdwt2){setData.PWMadjustValue[1] += setData.PWMadjustUnit;menu_tdwt2();}
		if(nowMenuIndex==tdwt3){setData.PWMadjustValue[2] += setData.PWMadjustUnit;menu_tdwt3();}
		if(nowMenuIndex==tdwt4){setData.PWMadjustValue[3] += setData.PWMadjustUnit;menu_tdwt4();}
		if(nowMenuIndex==tdwt5){setData.PWMadjustValue[4] += setData.PWMadjustUnit;menu_tdwt5();}
		if(nowMenuIndex==tdwt6){setData.PWMadjustValue[5] += setData.PWMadjustUnit;menu_tdwt6();}
		if(nowMenuIndex==tdwt7){setData.PWMadjustValue[6] += setData.PWMadjustUnit;menu_tdwt7();}
		if(nowMenuIndex==tdwt8){setData.PWMadjustValue[7] += setData.PWMadjustUnit;menu_tdwt8();}
		if(nowMenuIndex==tdzf1){setData.chReverse[0] = !setData.chReverse[0];menu_tdzf1();}
		if(nowMenuIndex==tdzf2){setData.chReverse[1] = !setData.chReverse[1];menu_tdzf2();}
		if(nowMenuIndex==tdzf3){setData.chReverse[2] = !setData.chReverse[2];menu_tdzf3();}
		if(nowMenuIndex==tdzf4){setData.chReverse[3] = !setData.chReverse[3];menu_tdzf4();}
		if(nowMenuIndex==tdzf5){setData.chReverse[4] = !setData.chReverse[4];menu_tdzf5();}
		if(nowMenuIndex==tdzf6){setData.chReverse[5] = !setData.chReverse[5];menu_tdzf6();}
		if(nowMenuIndex==tdzf7){setData.chReverse[6] = !setData.chReverse[6];menu_tdzf7();}
		if(nowMenuIndex==tdzf8){setData.chReverse[7] = !setData.chReverse[7];menu_tdzf8();}
		if(nowMenuIndex==ymph) {setData.throttlePreference = !setData.throttlePreference;menu_ymph();}
		if(nowMenuIndex==dyjz) {setData.batVoltAdjust += 1;menu_dyjz();}
		if(nowMenuIndex==bjdy) {setData.warnBatVolt += 0.01;menu_bjdy();}
		if(nowMenuIndex==wtdw) {setData.PWMadjustUnit += 1;menu_wtdw();}
		if(nowMenuIndex==xzmx) {setData.modelType += 1;if(setData.modelType>2) {setData.modelType=0;}menu_xzmx();}
		if(nowMenuIndex==wxfs) {setData.NRF_Mode =!setData.NRF_Mode;menu_wxfs();}
	}
	if(menuEvent[1]==NUM_down)
	{
		if(nowMenuIndex==tdwt1){setData.PWMadjustValue[0] -= setData.PWMadjustUnit;menu_tdwt1();}
		if(nowMenuIndex==tdwt2){setData.PWMadjustValue[1] -= setData.PWMadjustUnit;menu_tdwt2();}
		if(nowMenuIndex==tdwt3){setData.PWMadjustValue[2] -= setData.PWMadjustUnit;menu_tdwt3();}
		if(nowMenuIndex==tdwt4){setData.PWMadjustValue[3] -= setData.PWMadjustUnit;menu_tdwt4();}
		if(nowMenuIndex==tdwt5){setData.PWMadjustValue[4] -= setData.PWMadjustUnit;menu_tdwt5();}
		if(nowMenuIndex==tdwt6){setData.PWMadjustValue[5] -= setData.PWMadjustUnit;menu_tdwt6();}
		if(nowMenuIndex==tdwt7){setData.PWMadjustValue[6] -= setData.PWMadjustUnit;menu_tdwt7();}
		if(nowMenuIndex==tdwt8){setData.PWMadjustValue[7] -= setData.PWMadjustUnit;menu_tdwt8();}
		if(nowMenuIndex==tdzf1){setData.chReverse[0] = !setData.chReverse[0];menu_tdzf1();}
		if(nowMenuIndex==tdzf2){setData.chReverse[1] = !setData.chReverse[1];menu_tdzf2();}
		if(nowMenuIndex==tdzf3){setData.chReverse[2] = !setData.chReverse[2];menu_tdzf3();}
		if(nowMenuIndex==tdzf4){setData.chReverse[3] = !setData.chReverse[3];menu_tdzf4();}
		if(nowMenuIndex==tdzf5){setData.chReverse[4] = !setData.chReverse[4];menu_tdzf5();}
		if(nowMenuIndex==tdzf6){setData.chReverse[5] = !setData.chReverse[5];menu_tdzf6();}
		if(nowMenuIndex==tdzf7){setData.chReverse[6] = !setData.chReverse[6];menu_tdzf7();}
		if(nowMenuIndex==tdzf8){setData.chReverse[7] = !setData.chReverse[7];menu_tdzf8();}
		if(nowMenuIndex==ymph) {setData.throttlePreference = !setData.throttlePreference;menu_ymph();}
		if(nowMenuIndex==dyjz) {setData.batVoltAdjust -= 1;menu_dyjz();}
		if(nowMenuIndex==bjdy) {setData.warnBatVolt -= 0.01;menu_bjdy();}
		if(nowMenuIndex==wtdw) {setData.PWMadjustUnit -= 1;menu_wtdw();}
		if(nowMenuIndex==xzmx) {if(setData.modelType==0){setData.modelType=2;}else {setData.modelType -= 1;}menu_xzmx();}
		if(nowMenuIndex==wxfs) {setData.NRF_Mode =!setData.NRF_Mode;menu_wxfs();}
	}
	if(menuEvent[1]==MENU_enter)//旋转编码器短按后，改变菜单显示
	{
		if(nowMenuIndex==tdwt1){menu_tdwt1();}
		if(nowMenuIndex==tdwt2){menu_tdwt2();}
		if(nowMenuIndex==tdwt3){menu_tdwt3();}
		if(nowMenuIndex==tdwt4){menu_tdwt4();}
		if(nowMenuIndex==tdwt5){menu_tdwt5();}
		if(nowMenuIndex==tdwt6){menu_tdwt6();}
		if(nowMenuIndex==tdwt7){menu_tdwt7();}
		if(nowMenuIndex==tdwt8){menu_tdwt8();}
		if(nowMenuIndex==tdzf1){menu_tdzf1();}
		if(nowMenuIndex==tdzf2){menu_tdzf2();}
		if(nowMenuIndex==tdzf3){menu_tdzf3();}
		if(nowMenuIndex==tdzf4){menu_tdzf4();}
		if(nowMenuIndex==tdzf5){menu_tdzf5();}
		if(nowMenuIndex==tdzf6){menu_tdzf6();}
		if(nowMenuIndex==tdzf7){menu_tdzf7();}
		if(nowMenuIndex==tdzf8){menu_tdzf8();}
		if(nowMenuIndex==ymph) {menu_ymph();}
		if(nowMenuIndex==dyjz) {menu_dyjz();}
		if(nowMenuIndex==bjdy) {menu_bjdy();}
		if(nowMenuIndex==wtdw) {menu_wtdw();}
		if(nowMenuIndex==xzmx) {menu_xzmx();}
		if(nowMenuIndex==wxfs) {menu_wxfs();}
	}
	if(nowMenuIndex!=lastMenuIndex)
	{
		STMFLASH_Write(FLASH_SAVE_ADDR,(u16 *)&setData,setDataSize);//将用户数据写入FLASH
	}
	OLED_Refresh_Gram();//刷新显存
	menuEvent[0] = 0;
}
