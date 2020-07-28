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
	//当前, 下, 上, 确定, home
    {0, 0, 0, 1, 0,(*mainWindow)},//一级界面
	{1, 2, 4, 5, 0,(*menu1)},//二级界面第一行
	{2, 3, 1, 9, 0,(*menu2)},//二级界面第二行
	{3, 4, 2, 13, 0,(*menu3)},//二级界面第三行
	{4, 1, 3, 0, 0,(*menu4)},//退出
	
	{5, 6, 8, 5, 0,(*subMenu1_1)},//三级界面,menu1选中第1行
	{6, 7, 5, 6, 0,(*subMenu1_2)},//menu1三级界面第2行
	{7, 8, 6, 7, 0,(*subMenu1_3)},//menu1三级界面第3行
	{8, 5, 7, 1, 0,(*subMenu1_4)},//menu1三级界面第4行
	
	{9 , 10, 12,  9, 0,(*subMenu2_1)},//menu2三级界面第1行
	{10, 11,  9, 10, 0,(*subMenu2_2)},//menu2三级界面第2行
	{11, 12, 10, 11, 0,(*subMenu2_3)},//menu2三级界面第3行
	{12,  9, 11, 2, 0,(*subMenu2_4)},//menu2三级界面第4行
	
	{13, 14, 16, 13, 0,(*subMenu3_1)},//menu3三级界面第1行
	{14, 15, 13, 14, 0,(*subMenu3_2)},//menu3三级界面第2行
	{15, 16, 14, 15, 0,(*subMenu3_3)},//menu3三级界面第3行
	{16, 13, 15, 3, 0,(*subMenu3_4)},//menu3三级界面第4行
};

u8 nowIndex = 0,lastIndex = 0;
extern unsigned char logo[];
extern unsigned char logoR[];

void OLED_display(void){
	switch(encoderEvent[1]){
		case BM_down: //逆时针旋转，向下
			nowIndex=Key_table[nowIndex].down_index;
		break;
		case BM_up: //顺时针旋转,向上
			nowIndex=Key_table[nowIndex].up_index;
		break;
		case KEY_DOWN://短按 确定进入
			nowIndex=Key_table[nowIndex].enter_index;
		break;
		case KEY_LONG://长按 返回主界面
			nowIndex=Key_table[nowIndex].esc_index;
		break;
	}
	if(nowIndex==0 && lastIndex>0) OLED_Fill(0,0,127,63,0);//清空
	if(nowIndex>=1 && nowIndex<=4 && (lastIndex>4 | lastIndex==0)) OLED_Fill(0,0,127,63,0);//清空
	if(nowIndex!=lastIndex)
	{
		Key_table[nowIndex].operate();
		OLED_Refresh_Gram();//刷新显存
	}
	lastIndex = nowIndex;
}


void mainWindow(void)
{
	u16 thrNum;
	int loca;
	//u8 txt[16]={0};//存放字符串文本
	//中间字母
	OLED_ShowString(35,2,(u8 *)"J-20",24,1);//主界面
	OLED_ShowString(20,35,(u8 *)"RADIO CONTROL",12,1);//主界面
	
	//第1通道微调标识-横滚-右横线
	OLED_Fill(66,59,124,62,0);//写0，清除原来的标志
	loca = (int)95+setData.PWMadjustValue[0]/4;
	OLED_Fill(loca,59,loca,62,1);//写1
	OLED_DrawPlusSign(95,61);//中心标识
	OLED_Fill(66,63,124,63,1);//写1
	
	//第2通道微调标识-升降舵-右竖线
	OLED_Fill(123,1,126,63,0);//写0
	loca = (int)32+setData.PWMadjustValue[1]/4;
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
	OLED_ShowString(0,0, (u8 *)"subMenu1_1  ",16,0);
	OLED_ShowString(0,16,(u8 *)"subMenu1_2  ",16,1);
	OLED_ShowString(0,32,(u8 *)"subMenu1_3  ",16,1);
	OLED_ShowString(0,48,(u8 *)"exit        ",16,1);
	OLED_Refresh_Gram();//刷新显存
}
void subMenu1_2(void)//子菜单列表
{
	OLED_ShowString(0,0, (u8 *)"subMenu1_1  ",16,1);
	OLED_ShowString(0,16,(u8 *)"subMenu1_2  ",16,0);
	OLED_ShowString(0,32,(u8 *)"subMenu1_3  ",16,1);
	OLED_ShowString(0,48,(u8 *)"exit        ",16,1);
	OLED_Refresh_Gram();//刷新显存
}
void subMenu1_3(void)//子菜单列表
{
	OLED_ShowString(0,0, (u8 *)"subMenu1_1  ",16,1);
	OLED_ShowString(0,16,(u8 *)"subMenu1_2  ",16,1);
	OLED_ShowString(0,32,(u8 *)"subMenu1_3  ",16,0);
	OLED_ShowString(0,48,(u8 *)"exit        ",16,1);
	OLED_Refresh_Gram();//刷新显存
}
void subMenu1_4(void)//子菜单列表
{
	OLED_ShowString(0,0, (u8 *)"subMenu1_1  ",16,1);
	OLED_ShowString(0,16,(u8 *)"subMenu1_2  ",16,1);
	OLED_ShowString(0,32,(u8 *)"subMenu1_3  ",16,1);
	OLED_ShowString(0,48,(u8 *)"exit        ",16,0);
	OLED_Refresh_Gram();//刷新显存
}
//======================子菜单2===========================
void subMenu2_1(void)//子菜单列表
{
	OLED_ShowString(0,0, (u8 *)"subMenu2_1  ",16,0);
	OLED_ShowString(0,16,(u8 *)"subMenu2_2  ",16,1);
	OLED_ShowString(0,32,(u8 *)"subMenu2_3  ",16,1);
	OLED_ShowString(0,48,(u8 *)"exit        ",16,1);
}
void subMenu2_2(void)//子菜单列表
{
	OLED_ShowString(0,0, (u8 *)"subMenu2_1  ",16,1);
	OLED_ShowString(0,16,(u8 *)"subMenu2_2  ",16,0);
	OLED_ShowString(0,32,(u8 *)"subMenu2_3  ",16,1);
	OLED_ShowString(0,48,(u8 *)"exit        ",16,1);
}
void subMenu2_3(void)//子菜单列表
{
	OLED_ShowString(0,0, (u8 *)"subMenu2_1  ",16,1);
	OLED_ShowString(0,16,(u8 *)"subMenu2_2  ",16,1);
	OLED_ShowString(0,32,(u8 *)"subMenu2_3  ",16,0);
	OLED_ShowString(0,48,(u8 *)"exit        ",16,1);
}
void subMenu2_4(void)//子菜单列表
{
	OLED_ShowString(0,0, (u8 *)"subMenu2_1  ",16,1);
	OLED_ShowString(0,16,(u8 *)"subMenu2_2  ",16,1);
	OLED_ShowString(0,32,(u8 *)"subMenu2_3  ",16,1);
	OLED_ShowString(0,48,(u8 *)"exit        ",16,0);
}
//======================子菜单3===========================
void subMenu3_1(void)//子菜单列表
{
	OLED_ShowString(0,0, (u8 *)"subMenu3_1  ",16,0);
	OLED_ShowString(0,16,(u8 *)"subMenu3_2  ",16,1);
	OLED_ShowString(0,32,(u8 *)"subMenu3_3  ",16,1);
	OLED_ShowString(0,48,(u8 *)"exit        ",16,1);
}
void subMenu3_2(void)//子菜单列表
{
	OLED_ShowString(0,0, (u8 *)"subMenu3_1  ",16,1);
	OLED_ShowString(0,16,(u8 *)"subMenu3_2  ",16,0);
	OLED_ShowString(0,32,(u8 *)"subMenu3_3  ",16,1);
	OLED_ShowString(0,48,(u8 *)"exit        ",16,1);
}
void subMenu3_3(void)//子菜单列表
{
	OLED_ShowString(0,0, (u8 *)"subMenu3_1  ",16,1);
	OLED_ShowString(0,16,(u8 *)"subMenu3_2  ",16,1);
	OLED_ShowString(0,32,(u8 *)"subMenu3_3  ",16,0);
	OLED_ShowString(0,48,(u8 *)"exit        ",16,1);
}
void subMenu3_4(void)//子菜单列表
{
	OLED_ShowString(0,0, (u8 *)"subMenu3_1  ",16,1);
	OLED_ShowString(0,16,(u8 *)"subMenu3_2  ",16,1);
	OLED_ShowString(0,32,(u8 *)"subMenu3_3  ",16,1);
	OLED_ShowString(0,48,(u8 *)"exit        ",16,0);
}


