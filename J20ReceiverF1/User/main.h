#ifndef __MAIN_H
#define __MAIN_H
#ifdef __cplusplus
extern "C" {
#endif

/*main.h统一定义引脚，便于移植*/
#include "stm32f10x.h"
#include "sys.h"

#define LED_Pin 				GPIO_Pin_13
#define LED_GPIO_Port 	GPIOC
#define LED 						PCout(13)

#define CH1_Pin 				GPIO_Pin_0
#define CH1_GPIO_Port 	GPIOA
#define CH2_Pin 				GPIO_Pin_1
#define CH2_GPIO_Port 	GPIOA
#define CH3_Pin 				GPIO_Pin_2
#define CH3_GPIO_Port 	GPIOA
#define CH4_Pin 				GPIO_Pin_3
#define CH4_GPIO_Port 	GPIOA
#define CH5_Pin 				GPIO_Pin_6
#define CH5_GPIO_Port 	GPIOA
#define CH6_Pin 				GPIO_Pin_7
#define CH6_GPIO_Port 	GPIOA
#define CH7_Pin 				GPIO_Pin_0
#define CH7_GPIO_Port 	GPIOB
#define CH8_Pin 				GPIO_Pin_1
#define CH8_GPIO_Port 	GPIOB

#define PPM_Pin 				GPIO_Pin_4
#define PPM_GPIO_Port 	GPIOA
#define PPM 						PAout(4)	// PA4

#define SBUS_Pin 				GPIO_Pin_6
#define SBUS_GPIO_Port 	GPIOB

#define NRF_CSN_Pin 				GPIO_Pin_12
#define NRF_CSN_GPIO_Port 	GPIOB
#define NRF_SCK_Pin 				GPIO_Pin_13
#define NRF_SCK_GPIO_Port 	GPIOB
#define NRF_MISO_Pin 				GPIO_Pin_14
#define NRF_MISO_GPIO_Port 	GPIOB
#define NRF_MOSI_Pin 				GPIO_Pin_15
#define NRF_MOSI_GPIO_Port 	GPIOB
#define NRF_CE_Pin 					GPIO_Pin_8
#define NRF_CE_GPIO_Port 		GPIOA
#define NRF_IRQ_Pin 				GPIO_Pin_9
#define NRF_IRQ_GPIO_Port 	GPIOA
//24L01操作线
#define NRF24L01_CE   PAout(8) //24L01片选信号
#define NRF24L01_CSN  PBout(12) //SPI片选信号	   
#define NRF24L01_IRQ  PAin(9)  //IRQ主机数据输入

#ifdef __cplusplus
}
#endif
#endif
