#ifndef __BEEPER_H
#define __BEEPER_H	 
#include "sys.h"

#define Beeper PAout(10)	//PA10

void BEEPER_Init(void);//初始化
void beeperOnce(u16 freq);//蜂鸣器短响一次
void keyDownSound(void);
void onSound(void);
#endif
