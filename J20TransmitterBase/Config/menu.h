#ifndef __MENU_H
#define __MENU_H			  	 
#include "sys.h"
#include "stdlib.h"

typedef struct{
	u8 current_index;	//存放当前界面的索引号；
	u8 down_index;		//存放按下“down（向下）”键时需要跳转到的索引号；
	u8 up_index;		//存放按下“up（向上）”键时需要跳转到的索引号；
	u8 enter_index;		//存放按下“enter（进入）”键时需要跳转的索引号；
	u8 esc_index;		//存放按下“esc（退出）”键时需要跳转的索引号；
	u8 home_index;		//存放按下“home（主界面）”键时需要跳转的索引号；
	void (*operate)();	//函数指针变量，存放当前索引号下需要执行的函数的入口地址。
}Key_index_struct;

typedef struct Lcd_menu_content{
	u8 *lpIcon; //显示图标
	u8 *lpText; //显示文本信息
	u8 nTextCount; //菜单对应的文本信息的个数
}Lcd_menu_content;

typedef struct Lcd_menu{
	struct Lcd_menu *lpfather; //父级
	struct Lcd_menu *lpson; //大儿子
	u8 nSonCount; //父级的儿子个数
	Lcd_menu_content lpIconAndText;
	u8 Flag_return; //返回标志
	void  (*operate)(); //处理函数入口地址
}Lcd_menu;
extern volatile u8 nowMenuIndex;
extern volatile u8 lastMenuIndex;

void OLED_display(void);
void mainWindow(void);//主界面
void menu1(void);//菜单列表
void menu2(void);//菜单列表
void menu3(void);//菜单列表
void menu4(void);//菜单列表

void subMenu1_1(void);//子菜单
void subMenu1_2(void);//子菜单
void subMenu1_3(void);//子菜单
void subMenu1_4(void);//子菜单

void subMenu2_1(void);//子菜单
void subMenu2_2(void);//子菜单
void subMenu2_3(void);//子菜单
void subMenu2_4(void);//子菜单

void subMenu3_1(void);//子菜单
void subMenu3_2(void);//子菜单
void subMenu3_3(void);//子菜单
void subMenu3_4(void);//子菜单

void subMenu4_1(void);//子菜单
void subMenu4_2(void);//子菜单
void subMenu4_3(void);//子菜单
void subMenu4_4(void);//子菜单
#endif

