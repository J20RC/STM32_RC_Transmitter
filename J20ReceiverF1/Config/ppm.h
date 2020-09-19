#ifndef __PPM_H
#define __PPM_H
#include "stm32f10x.h"
#include "sys.h"
#include "pwm.h"

#define PPM_Pin PAout(4)	// PA4

extern u32 chTime[chNum];

void PPM_Pin_Init(void);
void ppm_output(void);
void systick_init(u32 count);
void SysTick_Handler(void);
#endif

