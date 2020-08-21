#include "oled.h"
#include "stdlib.h" 	 
#include "delay.h"
#include "menu.h"
#include "nrf24l01.h"
#include "led.h"
#include "key.h"
#include "flash.h"
#include "adc.h"
#include "delay.h"
#include "usart.h"

/*https://blog.csdn.net/embedded_guzi/article/details/35835755
https://blog.csdn.net/calmuse/article/details/79346742
*/

Key_index_struct const Key_table[18]=
{
	//当前, 下, 上, 确定, 返回, home
    {0, 0, 0, 1, 0, 0,(*mainWindow)},//一级界面
	{1, 2, 4, 5, 0, 0,(*menu1)},//二级界面第一行 通道微调
	{2, 3, 1, 9, 0, 0,(*menu2)},//二级界面第二行 通道正反
	{3, 4, 2, 13,0, 0,(*menu3)},//二级界面第三行 通道校准
	{4, 1, 3, 14,0, 0,(*menu4)},//二级界面第四行 系统设置
	
	{5, 6, 8, 5, 1, 0,(*subMenu1_1)},//三级界面,menu1选中第1行 通道微调
	{6, 7, 5, 6, 1, 0,(*subMenu1_2)},//三级界面,menu1选中第2行
	{7, 8, 6, 7, 1, 0,(*subMenu1_3)},//三级界面,menu1选中第3行
	{8, 5, 7, 1, 1, 0,(*subMenu1_4)},//三级界面,menu1选中第4行
	
	{9 , 10, 12, 9,  2, 0,(*subMenu2_1)},//三级界面,menu2选中第1行 通道正反
	{10, 11,  9, 10, 2, 0,(*subMenu2_2)},//三级界面,menu2选中第2行
	{11, 12, 10, 11, 2, 0,(*subMenu2_3)},//三级界面,menu2选中第3行
	{12,  9, 11, 2,  2, 0,(*subMenu2_4)},//三级界面,menu2选中第4行
	
	{13, 13, 13, 13, 3, 0,(*subMenu3_1)},//三级界面,menu3选中第1行 通道校准
	//当前, 下, 上, 确定, 返回, home
	{14, 15, 17, 14, 4, 0,(*subMenu4_1)},//三级界面,menu4选中第1行 系统设置
	{15, 16, 14, 15, 4, 0,(*subMenu4_2)},
	{16, 17, 15, 16, 4, 0,(*subMenu4_3)},
	{17, 14, 16, 17, 4, 0,(*subMenu4_4)},
};

u8 volatile nowMenuIndex = 0;
u8 volatile lastMenuIndex = 0;
extern unsigned char logo[];
extern unsigned char logoR[];
char batVoltStr[10]={0};

void OLED_display(void){
	switch(menuEvent[1]){
		case BM_down: //逆时针旋转，向下
			nowMenuIndex=Key_table[nowMenuIndex].down_index;
			break;
		case BM_up: //顺时针旋转,向上
			nowMenuIndex=Key_table[nowMenuIndex].up_index;
			break;
		case KEY_enter://确定
			nowMenuIndex=Key_table[nowMenuIndex].enter_index;
			break;
		case KEY_esc://返回
			nowMenuIndex=Key_table[nowMenuIndex].esc_index;
			break;
		case KEY_home://长按 返回主界面
			nowMenuIndex=Key_table[nowMenuIndex].home_index;
			break;
		case NUM_down:
			break;
		case NUM_up:
			break;
	}
//	printf("key:%d, last:%d, now:%d\r\n",menuEvent[1],lastMenuIndex,nowMenuIndex);
	if(nowMenuIndex==0 && lastMenuIndex>0) OLED_Fill(0,0,127,63,0);//清空
	if(nowMenuIndex>=1 && nowMenuIndex<=4 && (lastMenuIndex>4 | lastMenuIndex<1)) OLED_Fill(0,0,127,63,0);//清空
	if(nowMenuIndex>=5 && nowMenuIndex<=8 && (lastMenuIndex>8 | lastMenuIndex<5)) OLED_Fill(0,0,127,63,0);//清空
	if(nowMenuIndex>=9 && nowMenuIndex<=12 && (lastMenuIndex>12 | lastMenuIndex<9)) OLED_Fill(0,0,127,63,0);//清空
	if(nowMenuIndex>=13 && nowMenuIndex<=13 && (lastMenuIndex<13)) OLED_Fill(0,0,127,63,0);//清空
	if(nowMenuIndex!=lastMenuIndex)
	{
		Key_table[nowMenuIndex].operate();
		OLED_Refresh_Gram();//刷新显存
	}
}


void mainWindow(void)
{
	u16 thrNum;
	int loca;
	//u8 txt[16]={0};//存放字符串文本
	//中间图标
	OLED_DrawPointBMP(9,0,logo,110,56,1);//显示logo
	
	sprintf((char *)batVoltStr,"%2.2fV",batVolt);
	OLED_ShowString(90,0, (u8 *)batVoltStr,12,1);//显示电池电压
	
	//第1通道微调标识-横滚-右横线
	OLED_Fill(66,59,124,62,0);//写0，清除原来的标志
	loca = (int)95+setData.PWMadjustValue[0]/4;
	OLED_Fill(loca,59,loca,62,1);//写1
	OLED_DrawPlusSign(95,61);//中心标识
	OLED_Fill(66,63,124,63,1);//写1
	
	//第2通道微调标识-升降舵-右竖线
	OLED_Fill(123,1,126,63,0);//写0
	loca = (int)32-setData.PWMadjustValue[1]/4;
	OLED_Fill(123,loca,126,loca,1);//写1
	OLED_DrawPlusSign(125,32);//中心标识
	OLED_Fill(127,1,127,63,1);//写1
	
	//第3通道油门行程线-左竖线
	thrNum = (int)(PWMvalue[2]-1000)/16;
	OLED_Fill(0,63-thrNum,0,63,1);//下部分写1
	OLED_Fill(0,0,0,63-thrNum,0);//上部分写0
	
	//第4通道微调标识-方向舵-左横线
	OLED_Fill(4,59,62,62,0);//写0，清除原来的标志
	loca = (int)33+setData.PWMadjustValue[3]/4;
	OLED_Fill(loca,59,loca,62,1);//写1
	OLED_DrawPlusSign(33,61);//中心标识
	OLED_Fill(4,63,62,63,1);//写1
}
void menu1(void)//菜单列表
{
	OLED_ShowChinese(0, 0,0,16,0);//通
	OLED_ShowChinese(16,0,1,16,0);//道
	OLED_ShowChinese(32,0,2,16,0);//微
	OLED_ShowChinese(48,0,3,16,0);//调
	
	OLED_ShowChinese(0, 16,0,16,1);//通
	OLED_ShowChinese(16,16,1,16,1);//道
	OLED_ShowChinese(32,16,4,16,1);//正
	OLED_ShowChinese(48,16,5,16,1);//反
	
	OLED_ShowChinese(0, 32,6,16,1);//行
	OLED_ShowChinese(16,32,7,16,1);//程
	OLED_ShowChinese(32,32,8,16,1);//校
	OLED_ShowChinese(48,32,9,16,1);//准
	
	OLED_ShowChinese(0, 48,12,16,1);//系
	OLED_ShowChinese(16,48,13,16,1);//统
	OLED_ShowChinese(32,48,14,16,1);//设
	OLED_ShowChinese(48,48,15,16,1);//置
}
void menu2(void)//菜单列表
{
	OLED_ShowChinese(0, 0,0,16,1);//通
	OLED_ShowChinese(16,0,1,16,1);//道
	OLED_ShowChinese(32,0,2,16,1);//微
	OLED_ShowChinese(48,0,3,16,1);//调
	
	OLED_ShowChinese(0, 16,0,16,0);//通
	OLED_ShowChinese(16,16,1,16,0);//道
	OLED_ShowChinese(32,16,4,16,0);//正
	OLED_ShowChinese(48,16,5,16,0);//反
	
	OLED_ShowChinese(0, 32,6,16,1);//行
	OLED_ShowChinese(16,32,7,16,1);//程
	OLED_ShowChinese(32,32,8,16,1);//校
	OLED_ShowChinese(48,32,9,16,1);//准
	
	OLED_ShowChinese(0, 48,12,16,1);//系
	OLED_ShowChinese(16,48,13,16,1);//统
	OLED_ShowChinese(32,48,14,16,1);//设
	OLED_ShowChinese(48,48,15,16,1);//置
}
void menu3(void)//菜单列表
{
	OLED_ShowChinese(0, 0,0,16,1);//通
	OLED_ShowChinese(16,0,1,16,1);//道
	OLED_ShowChinese(32,0,2,16,1);//微
	OLED_ShowChinese(48,0,3,16,1);//调
	
	OLED_ShowChinese(0, 16,0,16,1);//通
	OLED_ShowChinese(16,16,1,16,1);//道
	OLED_ShowChinese(32,16,4,16,1);//正
	OLED_ShowChinese(48,16,5,16,1);//反
	
	OLED_ShowChinese(0, 32,6,16,0);//行
	OLED_ShowChinese(16,32,7,16,0);//程
	OLED_ShowChinese(32,32,8,16,0);//校
	OLED_ShowChinese(48,32,9,16,0);//准
	
	OLED_ShowChinese(0, 48,12,16,1);//系
	OLED_ShowChinese(16,48,13,16,1);//统
	OLED_ShowChinese(32,48,14,16,1);//设
	OLED_ShowChinese(48,48,15,16,1);//置
}
void menu4(void)//菜单列表-退出
{
	OLED_ShowChinese(0, 0,0,16,1);//通
	OLED_ShowChinese(16,0,1,16,1);//道
	OLED_ShowChinese(32,0,2,16,1);//微
	OLED_ShowChinese(48,0,3,16,1);//调
	
	OLED_ShowChinese(0, 16,0,16,1);//通
	OLED_ShowChinese(16,16,1,16,1);//道
	OLED_ShowChinese(32,16,4,16,1);//正
	OLED_ShowChinese(48,16,5,16,1);//反
	
	OLED_ShowChinese(0, 32,6,16,1);//行
	OLED_ShowChinese(16,32,7,16,1);//程
	OLED_ShowChinese(32,32,8,16,1);//校
	OLED_ShowChinese(48,32,9,16,1);//准
	
	OLED_ShowChinese(0, 48,12,16,0);//系
	OLED_ShowChinese(16,48,13,16,0);//统
	OLED_ShowChinese(32,48,14,16,0);//设
	OLED_ShowChinese(48,48,15,16,0);//置
}
//=======================子菜单1==========================
void subMenu1_1(void)//子菜单列表
{
	OLED_ShowChinese(0, 0,0,16,0);//通
	OLED_ShowChinese(16,0,1,16,0);//道
	OLED_ShowString(32,0, (u8 *)"1",16,0);
	OLED_ShowNum(48,0,abs(setData.PWMadjustValue[0]),3,16);
	if(setData.PWMadjustValue[0]<0) OLED_ShowString(74,0, (u8 *)"-",16,1);
	else OLED_ShowString(74,0, (u8 *)"+",16,1);
	
	OLED_ShowChinese(0, 16,0,16,1);//通
	OLED_ShowChinese(16,16,1,16,1);//道
	OLED_ShowString(32,16, (u8 *)"2",16,1);
	OLED_ShowNum(48,16,abs(setData.PWMadjustValue[1]),3,16);
	if(setData.PWMadjustValue[1]<0) OLED_ShowString(74,16, (u8 *)"-",16,1);
	else OLED_ShowString(74,16, (u8 *)"+",16,1);
	
	OLED_ShowChinese(0, 32,0,16,1);//通
	OLED_ShowChinese(16,32,1,16,1);//道
	OLED_ShowString(32,32, (u8 *)"3",16,1);
	OLED_ShowNum(48,32,abs(setData.PWMadjustValue[2]),3,16);
	if(setData.PWMadjustValue[2]<0) OLED_ShowString(74,32, (u8 *)"-",16,1);
	else OLED_ShowString(74,32, (u8 *)"+",16,1);
	
	OLED_ShowChinese(0, 48,0,16,1);//通
	OLED_ShowChinese(16,48,1,16,1);//道
	OLED_ShowString(32,48, (u8 *)"4",16,1);
	OLED_ShowNum(48,48,abs(setData.PWMadjustValue[3]),3,16);
	if(setData.PWMadjustValue[3]<0) OLED_ShowString(74,48, (u8 *)"-",16,1);
	else OLED_ShowString(74,48, (u8 *)"+",16,1);
}
void subMenu1_2(void)//子菜单列表
{
	OLED_ShowChinese(0, 0,0,16,1);//通
	OLED_ShowChinese(16,0,1,16,1);//道
	OLED_ShowString(32,0, (u8 *)"1",16,1);
	OLED_ShowNum(48,0,abs(setData.PWMadjustValue[0]),3,16);
	if(setData.PWMadjustValue[0]<0) OLED_ShowString(74,0, (u8 *)"-",16,1);
	else OLED_ShowString(74,0, (u8 *)"+",16,1);
	
	OLED_ShowChinese(0, 16,0,16,0);//通
	OLED_ShowChinese(16,16,1,16,0);//道
	OLED_ShowString(32,16, (u8 *)"2",16,0);
	OLED_ShowNum(48,16,abs(setData.PWMadjustValue[1]),3,16);
	if(setData.PWMadjustValue[1]<0) OLED_ShowString(74,16, (u8 *)"-",16,1);
	else OLED_ShowString(74,16, (u8 *)"+",16,1);
	
	OLED_ShowChinese(0, 32,0,16,1);//通
	OLED_ShowChinese(16,32,1,16,1);//道
	OLED_ShowString(32,32, (u8 *)"3",16,1);
	OLED_ShowNum(48,32,abs(setData.PWMadjustValue[2]),3,16);
	if(setData.PWMadjustValue[2]<0) OLED_ShowString(74,32, (u8 *)"-",16,1);
	else OLED_ShowString(74,32, (u8 *)"+",16,1);
	
	OLED_ShowChinese(0, 48,0,16,1);//通
	OLED_ShowChinese(16,48,1,16,1);//道
	OLED_ShowString(32,48, (u8 *)"4",16,1);
	OLED_ShowNum(48,48,abs(setData.PWMadjustValue[3]),3,16);
	if(setData.PWMadjustValue[3]<0) OLED_ShowString(74,48, (u8 *)"-",16,1);
	else OLED_ShowString(74,48, (u8 *)"+",16,1);
}
void subMenu1_3(void)//子菜单列表
{
	OLED_ShowChinese(0, 0,0,16,1);//通
	OLED_ShowChinese(16,0,1,16,1);//道
	OLED_ShowString(32,0, (u8 *)"1",16,1);
	OLED_ShowNum(48,0,abs(setData.PWMadjustValue[0]),3,16);
	if(setData.PWMadjustValue[0]<0) OLED_ShowString(74,0, (u8 *)"-",16,1);
	else OLED_ShowString(74,0, (u8 *)"+",16,1);
	
	OLED_ShowChinese(0, 16,0,16,1);//通
	OLED_ShowChinese(16,16,1,16,1);//道
	OLED_ShowString(32,16, (u8 *)"2",16,1);
	OLED_ShowNum(48,16,abs(setData.PWMadjustValue[1]),3,16);
	if(setData.PWMadjustValue[1]<0) OLED_ShowString(74,16, (u8 *)"-",16,1);
	else OLED_ShowString(74,16, (u8 *)"+",16,1);
	
	OLED_ShowChinese(0, 32,0,16,0);//通
	OLED_ShowChinese(16,32,1,16,0);//道
	OLED_ShowString(32,32, (u8 *)"3",16,0);
	OLED_ShowNum(48,32,abs(setData.PWMadjustValue[2]),3,16);
	if(setData.PWMadjustValue[2]<0) OLED_ShowString(74,32, (u8 *)"-",16,1);
	else OLED_ShowString(74,32, (u8 *)"+",16,1);
	
	OLED_ShowChinese(0, 48,0,16,1);//通
	OLED_ShowChinese(16,48,1,16,1);//道
	OLED_ShowString(32,48, (u8 *)"4",16,1);
	OLED_ShowNum(48,48,abs(setData.PWMadjustValue[3]),3,16);
	if(setData.PWMadjustValue[3]<0) OLED_ShowString(74,48, (u8 *)"-",16,1);
	else OLED_ShowString(74,48, (u8 *)"+",16,1);
}
void subMenu1_4(void)//子菜单列表
{
	OLED_ShowChinese(0, 0,0,16,1);//通
	OLED_ShowChinese(16,0,1,16,1);//道
	OLED_ShowString(32,0, (u8 *)"1",16,1);
	OLED_ShowNum(48,0,abs(setData.PWMadjustValue[0]),3,16);
	if(setData.PWMadjustValue[0]<0) OLED_ShowString(74,0, (u8 *)"-",16,1);
	else OLED_ShowString(74,0, (u8 *)"+",16,1);
	
	OLED_ShowChinese(0, 16,0,16,1);//通
	OLED_ShowChinese(16,16,1,16,1);//道
	OLED_ShowString(32,16, (u8 *)"2",16,1);
	OLED_ShowNum(48,16,abs(setData.PWMadjustValue[1]),3,16);
	if(setData.PWMadjustValue[1]<0) OLED_ShowString(74,16, (u8 *)"-",16,1);
	else OLED_ShowString(74,16, (u8 *)"+",16,1);
	
	OLED_ShowChinese(0, 32,0,16,1);//通
	OLED_ShowChinese(16,32,1,16,1);//道
	OLED_ShowString(32,32, (u8 *)"3",16,1);
	OLED_ShowNum(48,32,abs(setData.PWMadjustValue[2]),3,16);
	if(setData.PWMadjustValue[2]<0) OLED_ShowString(74,32, (u8 *)"-",16,1);
	else OLED_ShowString(74,32, (u8 *)"+",16,1);
	
	OLED_ShowChinese(0, 48,0,16,0);//通
	OLED_ShowChinese(16,48,1,16,0);//道
	OLED_ShowString(32,48, (u8 *)"4",16,0);
	OLED_ShowNum(48,48,abs(setData.PWMadjustValue[3]),3,16);
	if(setData.PWMadjustValue[3]<0) OLED_ShowString(74,48, (u8 *)"-",16,1);
	else OLED_ShowString(74,48, (u8 *)"+",16,1);
}
//======================子菜单2 通道正反===========================
void subMenu2_1(void)//子菜单列表
{
	OLED_ShowChinese(0, 0,0,16,0);//通
	OLED_ShowChinese(16,0,1,16,0);//道
	OLED_ShowString(32,0, (u8 *)"1",16,0);
	if(setData.chReverse[0])OLED_ShowChinese(48,0,4,16,1);//正
	else OLED_ShowChinese(48,0,5,16,1);//反
	
	OLED_ShowChinese(0, 16,0,16,1);//通
	OLED_ShowChinese(16,16,1,16,1);//道
	OLED_ShowString(32,16, (u8 *)"2",16,1);
	if(setData.chReverse[1])OLED_ShowChinese(48,16,4,16,1);//正
	else OLED_ShowChinese(48,16,5,16,1);//反
	
	OLED_ShowChinese(0, 32,0,16,1);//通
	OLED_ShowChinese(16,32,1,16,1);//道
	OLED_ShowString(32,32, (u8 *)"3",16,1);
	if(setData.chReverse[2])OLED_ShowChinese(48,32,4,16,1);//正
	else OLED_ShowChinese(48,32,5,16,1);//反
	
	OLED_ShowChinese(0, 48,0,16,1);//通
	OLED_ShowChinese(16,48,1,16,1);//道
	OLED_ShowString(32,48, (u8 *)"4",16,1);
	if(setData.chReverse[3])OLED_ShowChinese(48,48,4,16,1);//正
	else OLED_ShowChinese(48,48,5,16,1);//反
}
void subMenu2_2(void)//子菜单列表
{
	OLED_ShowChinese(0, 0,0,16,1);//通
	OLED_ShowChinese(16,0,1,16,1);//道
	OLED_ShowString(32,0, (u8 *)"1",16,1);
	if(setData.chReverse[0])OLED_ShowChinese(48,0,4,16,1);//正
	else OLED_ShowChinese(48,0,5,16,1);//反
	
	OLED_ShowChinese(0, 16,0,16,0);//通
	OLED_ShowChinese(16,16,1,16,0);//道
	OLED_ShowString(32,16, (u8 *)"2",16,0);
	if(setData.chReverse[1])OLED_ShowChinese(48,16,4,16,1);//正
	else OLED_ShowChinese(48,16,5,16,1);//反
	
	OLED_ShowChinese(0, 32,0,16,1);//通
	OLED_ShowChinese(16,32,1,16,1);//道
	OLED_ShowString(32,32, (u8 *)"3",16,1);
	if(setData.chReverse[2])OLED_ShowChinese(48,32,4,16,1);//正
	else OLED_ShowChinese(48,32,5,16,1);//反
	
	OLED_ShowChinese(0, 48,0,16,1);//通
	OLED_ShowChinese(16,48,1,16,1);//道
	OLED_ShowString(32,48, (u8 *)"4",16,1);
	if(setData.chReverse[3])OLED_ShowChinese(48,48,4,16,1);//正
	else OLED_ShowChinese(48,48,5,16,1);//反
}
void subMenu2_3(void)//子菜单列表
{
	OLED_ShowChinese(0, 0,0,16,1);//通
	OLED_ShowChinese(16,0,1,16,1);//道
	OLED_ShowString(32,0, (u8 *)"1",16,1);
	if(setData.chReverse[0])OLED_ShowChinese(48,0,4,16,1);//正
	else OLED_ShowChinese(48,0,5,16,1);//反
	
	OLED_ShowChinese(0, 16,0,16,1);//通
	OLED_ShowChinese(16,16,1,16,1);//道
	OLED_ShowString(32,16, (u8 *)"2",16,1);
	if(setData.chReverse[1])OLED_ShowChinese(48,16,4,16,1);//正
	else OLED_ShowChinese(48,16,5,16,1);//反
	
	OLED_ShowChinese(0, 32,0,16,0);//通
	OLED_ShowChinese(16,32,1,16,0);//道
	OLED_ShowString(32,32, (u8 *)"3",16,0);
	if(setData.chReverse[2])OLED_ShowChinese(48,32,4,16,1);//正
	else OLED_ShowChinese(48,32,5,16,1);//反
	
	OLED_ShowChinese(0, 48,0,16,1);//通
	OLED_ShowChinese(16,48,1,16,1);//道
	OLED_ShowString(32,48, (u8 *)"4",16,1);
	if(setData.chReverse[3])OLED_ShowChinese(48,48,4,16,1);//正
	else OLED_ShowChinese(48,48,5,16,1);//反
}
void subMenu2_4(void)//子菜单列表
{
	OLED_ShowChinese(0, 0,0,16,1);//通
	OLED_ShowChinese(16,0,1,16,1);//道
	OLED_ShowString(32,0, (u8 *)"1",16,1);
	if(setData.chReverse[0])OLED_ShowChinese(48,0,4,16,1);//正
	else OLED_ShowChinese(48,0,5,16,1);//反
	
	OLED_ShowChinese(0, 16,0,16,1);//通
	OLED_ShowChinese(16,16,1,16,1);//道
	OLED_ShowString(32,16, (u8 *)"2",16,1);
	if(setData.chReverse[1])OLED_ShowChinese(48,16,4,16,1);//正
	else OLED_ShowChinese(48,16,5,16,1);//反
	
	OLED_ShowChinese(0, 32,0,16,1);//通
	OLED_ShowChinese(16,32,1,16,1);//道
	OLED_ShowString(32,32, (u8 *)"3",16,1);
	if(setData.chReverse[2])OLED_ShowChinese(48,32,4,16,1);//正
	else OLED_ShowChinese(48,32,5,16,1);//反
	
	OLED_ShowChinese(0, 48,0,16,0);//通
	OLED_ShowChinese(16,48,1,16,0);//道
	OLED_ShowString(32,48, (u8 *)"4",16,0);
	if(setData.chReverse[3])OLED_ShowChinese(48,48,4,16,1);//正
	else OLED_ShowChinese(48,48,5,16,1);//反
}
//======================子菜单3行程校准===========================
void subMenu3_1(void)//子菜单列表
{
	OLED_ShowString(2,0, (u8 *)"1:",16,1);
	OLED_ShowNum(18,0,setData.chLower[0],4,16);
	OLED_ShowNum(56,0,setData.chMiddle[0],4,16);
	OLED_ShowNum(95,0,setData.chUpper[0],4,16);
	
	OLED_ShowString(2,16, (u8 *)"2:",16,1);
	OLED_ShowNum(18,16,setData.chLower[1],4,16);
	OLED_ShowNum(56,16,setData.chMiddle[1],4,16);
	OLED_ShowNum(95,16,setData.chUpper[1],4,16);
	
	OLED_ShowString(2,32, (u8 *)"3:",16,1);
	OLED_ShowNum(18,32,setData.chLower[2],4,16);
	OLED_ShowNum(56,32,setData.chMiddle[2],4,16);
	OLED_ShowNum(95,32,setData.chUpper[2],4,16);
	
	OLED_ShowString(2,48, (u8 *)"4:",16,1);
	OLED_ShowNum(18,48,setData.chLower[3],4,16);
	OLED_ShowNum(56,48,setData.chMiddle[3],4,16);
	OLED_ShowNum(95,48,setData.chUpper[3],4,16);
}

//======================子菜单4===========================
void subMenu4_1(void)//子菜单列表
{
	OLED_ShowChinese(0, 0,16,16,0);//油
	OLED_ShowChinese(16,0,17,16,0);//门
	OLED_ShowChinese(32,0,18,16,0);//偏
	OLED_ShowChinese(48,0,19,16,0);//好
	if(setData.throttlePreference)OLED_ShowChinese(80,0,26,16,1);//左
	else OLED_ShowChinese(80,0,27,16,1);//右
	OLED_ShowChinese(96,0,28,16,1);//手
	
	OLED_ShowChinese(0, 16,20,16,1);//电
	OLED_ShowChinese(16,16,21,16,1);//压
	OLED_ShowChinese(32,16, 8,16,1);//校
	OLED_ShowChinese(48,16, 9,16,1);//准
	sprintf((char *)batVoltStr,"%2.2fV",batVolt);
	OLED_ShowString(80,16, (u8 *)batVoltStr,16,1);
	
	OLED_ShowChinese(0, 32,22,16,1);//报
	OLED_ShowChinese(16,32,23,16,1);//警
	OLED_ShowChinese(32,32,20,16,1);//电
	OLED_ShowChinese(48,32,21,16,1);//压
	sprintf((char *)batVoltStr,"%2.2fV",setData.warnBatVolt);
	OLED_ShowString(80,32, (u8 *)batVoltStr,16,1);
	
	OLED_ShowChinese(0, 48, 2,16,1);//微
	OLED_ShowChinese(16,48, 3,16,1);//调
	OLED_ShowChinese(32,48,24,16,1);//单
	OLED_ShowChinese(48,48,25,16,1);//位
	OLED_ShowNum(80,48,setData.PWMadjustUnit,1,16);
}
void subMenu4_2(void)//子菜单列表
{
	OLED_ShowChinese(0, 0,16,16,1);//油
	OLED_ShowChinese(16,0,17,16,1);//门
	OLED_ShowChinese(32,0,18,16,1);//偏
	OLED_ShowChinese(48,0,19,16,1);//好
	if(setData.throttlePreference)OLED_ShowChinese(80,0,26,16,1);//左
	else OLED_ShowChinese(80,0,27,16,1);//右
	OLED_ShowChinese(96,0,28,16,1);//手
	
	OLED_ShowChinese(0, 16,20,16,0);//电
	OLED_ShowChinese(16,16,21,16,0);//压
	OLED_ShowChinese(32,16, 8,16,0);//校
	OLED_ShowChinese(48,16, 9,16,0);//准
	sprintf((char *)batVoltStr,"%2.2fV",batVolt);
	OLED_ShowString(80,16, (u8 *)batVoltStr,16,1);
	
	OLED_ShowChinese(0, 32,22,16,1);//报
	OLED_ShowChinese(16,32,23,16,1);//警
	OLED_ShowChinese(32,32,20,16,1);//电
	OLED_ShowChinese(48,32,21,16,1);//压
	sprintf((char *)batVoltStr,"%2.2fV",setData.warnBatVolt);
	OLED_ShowString(80,32, (u8 *)batVoltStr,16,1);
	
	OLED_ShowChinese(0, 48, 2,16,1);//微
	OLED_ShowChinese(16,48, 3,16,1);//调
	OLED_ShowChinese(32,48,24,16,1);//单
	OLED_ShowChinese(48,48,25,16,1);//位
	OLED_ShowNum(80,48,setData.PWMadjustUnit,1,16);
}
void subMenu4_3(void)//子菜单列表
{
	OLED_ShowChinese(0, 0,16,16,1);//油
	OLED_ShowChinese(16,0,17,16,1);//门
	OLED_ShowChinese(32,0,18,16,1);//偏
	OLED_ShowChinese(48,0,19,16,1);//好
	if(setData.throttlePreference)OLED_ShowChinese(80,0,26,16,1);//左
	else OLED_ShowChinese(80,0,27,16,1);//右
	OLED_ShowChinese(96,0,28,16,1);//手
	
	OLED_ShowChinese(0, 16,20,16,1);//电
	OLED_ShowChinese(16,16,21,16,1);//压
	OLED_ShowChinese(32,16, 8,16,1);//校
	OLED_ShowChinese(48,16, 9,16,1);//准
	sprintf((char *)batVoltStr,"%2.2fV",batVolt);
	OLED_ShowString(80,16, (u8 *)batVoltStr,16,1);
	
	OLED_ShowChinese(0, 32,22,16,0);//报
	OLED_ShowChinese(16,32,23,16,0);//警
	OLED_ShowChinese(32,32,20,16,0);//电
	OLED_ShowChinese(48,32,21,16,0);//压
	sprintf((char *)batVoltStr,"%2.2fV",setData.warnBatVolt);
	OLED_ShowString(80,32, (u8 *)batVoltStr,16,1);
	
	OLED_ShowChinese(0, 48, 2,16,1);//微
	OLED_ShowChinese(16,48, 3,16,1);//调
	OLED_ShowChinese(32,48,24,16,1);//单
	OLED_ShowChinese(48,48,25,16,1);//位
	OLED_ShowNum(80,48,setData.PWMadjustUnit,1,16);
}
void subMenu4_4(void)//子菜单列表
{
	OLED_ShowChinese(0, 0,16,16,1);//油
	OLED_ShowChinese(16,0,17,16,1);//门
	OLED_ShowChinese(32,0,18,16,1);//偏
	OLED_ShowChinese(48,0,19,16,1);//好
	if(setData.throttlePreference)OLED_ShowChinese(80,0,26,16,1);//左
	else OLED_ShowChinese(80,0,27,16,1);//右
	OLED_ShowChinese(96,0,28,16,1);//手
	
	OLED_ShowChinese(0, 16,20,16,1);//电
	OLED_ShowChinese(16,16,21,16,1);//压
	OLED_ShowChinese(32,16, 8,16,1);//校
	OLED_ShowChinese(48,16, 9,16,1);//准
	sprintf((char *)batVoltStr,"%2.2fV",batVolt);
	OLED_ShowString(80,16, (u8 *)batVoltStr,16,1);
	
	OLED_ShowChinese(0, 32,22,16,1);//报
	OLED_ShowChinese(16,32,23,16,1);//警
	OLED_ShowChinese(32,32,20,16,1);//电
	OLED_ShowChinese(48,32,21,16,1);//压
	sprintf((char *)batVoltStr,"%2.2fV",setData.warnBatVolt);
	OLED_ShowString(80,32, (u8 *)batVoltStr,16,1);
	
	OLED_ShowChinese(0, 48, 2,16,0);//微
	OLED_ShowChinese(16,48, 3,16,0);//调
	OLED_ShowChinese(32,48,24,16,0);//单
	OLED_ShowChinese(48,48,25,16,0);//位
	OLED_ShowNum(80,48,setData.PWMadjustUnit,1,16);
}
