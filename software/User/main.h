#ifndef __MAIN_H
#define __MAIN_H
#ifdef __cplusplus
extern "C" {
#endif

/*main.h统一定义引脚，便于移植*/
#include "adc.h"
#include "delay.h"
#include "usart.h"
#include "rtc.h"
#include "stm32f10x.h"
#include "oled.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "nrf24l01.h"
#include "beeper.h"
#include "key.h"
#include "flash.h"
#include "menu.h"
#include "ppm.h"
#include "tim.h"
#include "setting.h"

#define ON			1
#define OFF			0

#define FM_FLAG		0x0221
#define FM_VERSION	"V2.2.1"
#define FM_TIME		"2021.03.02"

#define PPM_Pin         GPIO_Pin_13 //PPM输出脚
#define PPM_GPIO_Port   GPIOC        
#define PPM             PCout(13)

#ifdef __cplusplus
}
#endif
#endif
