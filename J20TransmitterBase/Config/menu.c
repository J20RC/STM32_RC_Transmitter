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

Key_index_struct const Key_table[17]=
{
	//当前, 下, 上, 确定, 返回, home
    {0, 0, 0, 1, 0, 0,(*mainWindow)},//一级界面
	{1, 2, 4, 5, 0, 0,(*menu1)},//二级界面第一行 通道微调
	{2, 3, 1, 9, 0, 0,(*menu2)},//二级界面第二行 通道正反
	{3, 4, 2, 13,0, 0,(*menu3)},//二级界面第三行 通道校准
	{4, 1, 3, 0, 0, 0,(*menu4)},//退出
	
	{5, 6, 8, 5, 1, 0,(*subMenu1_1)},//三级界面,menu1选中第1行 通道微调
	{6, 7, 5, 6, 1, 0,(*subMenu1_2)},//三级界面,menu1选中第2行
	{7, 8, 6, 7, 1, 0,(*subMenu1_3)},//三级界面,menu1选中第3行
	{8, 5, 7, 1, 1, 0,(*subMenu1_4)},//三级界面,menu1选中第4行
	
	{9 , 10, 12, 9,  2, 0,(*subMenu2_1)},//三级界面,menu2选中第1行 通道正反
	{10, 11,  9, 10, 2, 0,(*subMenu2_2)},//三级界面,menu2选中第2行
	{11, 12, 10, 11, 2, 0,(*subMenu2_3)},//三级界面,menu2选中第3行
	{12,  9, 11, 2,  2, 0,(*subMenu2_4)},//三级界面,menu2选中第4行
	
	{13, 13, 13, 13, 3, 0,(*subMenu3_1)},//三级界面,menu3选中第1行 通道校准
};

u8 nowIndex = 0,lastIndex = 0;
extern unsigned char logo[];
extern unsigned char logoR[];

void OLED_display(void){
	switch(menuEvent[1]){
		case BM_down: //逆时针旋转，向下
			nowIndex=Key_table[nowIndex].down_index;
			break;
		case BM_up: //顺时针旋转,向上
			nowIndex=Key_table[nowIndex].up_index;
			break;
		case KEY_enter://确定
			nowIndex=Key_table[nowIndex].enter_index;
			break;
		case KEY_esc://返回
			nowIndex=Key_table[nowIndex].esc_index;
			break;
		case KEY_home://长按 返回主界面
			nowIndex=Key_table[nowIndex].home_index;
			break;
		case NUM_down:
			break;
		case NUM_up:
			break;
	}
	if(nowIndex==0 && lastIndex>0) OLED_Fill(0,0,127,63,0);//清空
	if(nowIndex>=1 && nowIndex<=4 && (lastIndex>4 | lastIndex<1)) OLED_Fill(0,0,127,63,0);//清空
	if(nowIndex>=5 && nowIndex<=8 && (lastIndex>8 | lastIndex<5)) OLED_Fill(0,0,127,63,0);//清空
	if(nowIndex>=9 && nowIndex<=12 && (lastIndex>12 | lastIndex<9)) OLED_Fill(0,0,127,63,0);//清空
	if(nowIndex>=13 && nowIndex<=13 && (lastIndex<13)) OLED_Fill(0,0,127,63,0);//清空
	if(nowIndex!=lastIndex)
	{
		Key_table[nowIndex].operate();
		OLED_Refresh_Gram();//刷新显存
	}
	printf("now:%d, last:%d, event:%d\n",nowIndex,lastIndex,menuEvent[1]);
	lastIndex = nowIndex;
}


void mainWindow(void)
{
	u16 thrNum;
	int loca;
	//u8 txt[16]={0};//存放字符串文本
	//中间图标
	OLED_DrawPointBMP(9,0,logo,110,56,1);//显示logo
	
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
	
	OLED_ShowChinese(0, 48,10,16,1);//退
	OLED_ShowChinese(16,48,11,16,1);//出
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
	
	OLED_ShowChinese(0, 48,10,16,1);//退
	OLED_ShowChinese(16,48,11,16,1);//出
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
	
	OLED_ShowChinese(0, 48,10,16,1);//退
	OLED_ShowChinese(16,48,11,16,1);//出
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
	
	OLED_ShowChinese(0, 48,10,16,0);//退
	OLED_ShowChinese(16,48,11,16,0);//出
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
//======================子菜单3===========================
void subMenu3_1(void)//子菜单列表
{
	OLED_ShowChinese(0, 0,0,16,1);//通
	OLED_ShowChinese(16,0,1,16,1);//道
	OLED_ShowString(32,0, (u8 *)"1",16,1);
	OLED_ShowNum(40,0,setData.chLower[0],4,16);
	OLED_ShowNum(80,0,setData.chUpper[0],4,16);
	
	OLED_ShowChinese(0, 16,0,16,1);//通
	OLED_ShowChinese(16,16,1,16,1);//道
	OLED_ShowString(32,16, (u8 *)"2",16,1);
	OLED_ShowNum(40,16,setData.chLower[1],4,16);
	OLED_ShowNum(80,16,setData.chUpper[1],4,16);
	
	OLED_ShowChinese(0, 32,0,16,1);//通
	OLED_ShowChinese(16,32,1,16,1);//道
	OLED_ShowString(32,32, (u8 *)"3",16,1);
	OLED_ShowNum(40,32,setData.chLower[2],4,16);
	OLED_ShowNum(80,32,setData.chUpper[2],4,16);
	
	OLED_ShowChinese(0, 48,0,16,1);//通
	OLED_ShowChinese(16,48,1,16,1);//道
	OLED_ShowString(32,48, (u8 *)"4",16,1);
	OLED_ShowNum(40,48,setData.chLower[3],4,16);
	OLED_ShowNum(80,48,setData.chUpper[3],4,16);
}
