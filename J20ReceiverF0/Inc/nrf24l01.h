#ifndef __nrf24L01_H
#define __nrf24L01_H
 
#include "stdint.h"
 
 
/* 类型定义 ------------------------------------------------------------------*/
/* 宏定义 --------------------------------------------------------------------*/
#define NRF24L01_SPIx                                 SPI1
#define NRF24L01_SPIx_RCC_CLK_ENABLE()                __HAL_RCC_SPI1_CLK_ENABLE()
#define NRF24L01_SPIx_RCC_CLK_DISABLE()               __HAL_RCC_SPI1_CLK_DISABLE()
 
#define NRF24L01_SPI_GPIO_ClK_ENABLE()                __HAL_RCC_GPIOA_CLK_ENABLE() 

#define NRF24L01_SPI_GPIO_PORT                        GPIOB
#define NRF24L01_SPI_SCK_PIN                          GPIO_PIN_3
#define NRF24L01_SPI_MISO_PIN                         GPIO_PIN_4
#define NRF24L01_SPI_MOSI_PIN                         GPIO_PIN_5
 
#define NRF24L01_SPI_CS_CLK_ENABLE()                  __HAL_RCC_GPIOB_CLK_ENABLE()    
#define NRF24L01_SPI_CS_PORT                          GPIOA
#define NRF24L01_SPI_CS_PIN                           GPIO_PIN_15
#define NRF24L01_SPI_CS_ENABLE()                      HAL_GPIO_WritePin(NRF24L01_SPI_CS_PORT, NRF24L01_SPI_CS_PIN, GPIO_PIN_RESET)
#define NRF24L01_SPI_CS_DISABLE()                     HAL_GPIO_WritePin(NRF24L01_SPI_CS_PORT, NRF24L01_SPI_CS_PIN, GPIO_PIN_SET)
 
#define NRF24L01_CE_CLK_ENABLE()                      __HAL_RCC_GPIOB_CLK_ENABLE()    
#define NRF24L01_CE_PORT                              GPIOA
#define NRF24L01_CE_PIN                               GPIO_PIN_4
#define NRF24L01_CE_LOW()                             HAL_GPIO_WritePin(NRF24L01_CE_PORT, NRF24L01_CE_PIN, GPIO_PIN_RESET)
#define NRF24L01_CE_HIGH()                            HAL_GPIO_WritePin(NRF24L01_CE_PORT, NRF24L01_CE_PIN, GPIO_PIN_SET)
 
#define NRF24L01_IRQ_CLK_ENABLE()                     __HAL_RCC_GPIOA_CLK_ENABLE()    
#define NRF24L01_IRQ_PORT                             GPIOA
#define NRF24L01_IRQ_PIN                              GPIO_PIN_5
#define NRF24L01_IRQ_PIN_READ()                       HAL_GPIO_ReadPin(NRF24L01_IRQ_PORT,NRF24L01_IRQ_PIN)
 
 

//NRF24L01寄存器操作命令
#define NRF_READ_REG                                  0x00  //读配置寄存器,低5位为寄存器地址
#define NRF_WRITE_REG                                 0x20  //写配置寄存器,低5位为寄存器地址
#define RD_RX_PLOAD                                   0x61  //读RX有效数据,1~32字节
#define WR_TX_PLOAD                                   0xA0  //写TX有效数据,1~32字节
#define FLUSH_TX                                      0xE1  //清除TX FIFO寄存器.发射模式下用
#define FLUSH_RX                                      0xE2  //清除RX FIFO寄存器.接收模式下用
#define REUSE_TX_PL                                   0xE3  //重新使用上一包数据,CE为高,数据包被不断发送.
#define NOP                                           0xFF  //空操作,可以用来读状态寄存器	 
//SPI(NRF24L01)寄存器地址
#define CONFIG                                        0x00  //配置寄存器地址;bit0:1接收模式,0发射模式;bit1:电选择;bit2:CRC模式;bit3:CRC使能;
                                                            //bit4:中断MAX_RT(达到最大重发次数中断)使能;bit5:中断TX_DS使能;bit6:中断RX_DR使能
#define EN_AA                                         0x01  //使能自动应答功能  bit0~5,对应通道0~5
#define EN_RXADDR                                     0x02  //接收地址允许,bit0~5,对应通道0~5
#define SETUP_AW                                      0x03  //设置地址宽度(所有数据通道):bit1,0:00,3字节;01,4字节;02,5字节;
#define SETUP_RETR                                    0x04  //建立自动重发;bit3:0,自动重发计数器;bit7:4,自动重发延时 250*x+86us
#define RF_CH                                         0x05  //RF通道,bit6:0,工作通道频率;
#define RF_SETUP                                      0x06  //RF寄存器;bit3:传输速率(0:1Mbps,1:2Mbps);bit2:1,发射功率;bit0:低噪声放大器增益
#define STATUS                                        0x07  //状态寄存器;bit0:TX FIFO满标志;bit3:1,接收数据通道号(最大:6);bit4,达到最多次重发
                                                            //bit5:数据发送完成中断;bit6:接收数据中断;
#define MAX_TX  		                                  0x10  //达到最大发送次数中断
#define TX_OK   		                                  0x20  //TX发送完成中断
#define RX_OK   		                                  0x40  //接收到数据中断
 
#define OBSERVE_TX                                    0x08  //发送检测寄存器,bit7:4,数据包丢失计数器;bit3:0,重发计数器
#define CD                                            0x09  //载波检测寄存器,bit0,载波检测;
#define RX_ADDR_P0                                    0x0A  //数据通道0接收地址,最大长度5个字节,低字节在前
#define RX_ADDR_P1                                    0x0B  //数据通道1接收地址,最大长度5个字节,低字节在前
#define RX_ADDR_P2                                    0x0C  //数据通道2接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define RX_ADDR_P3                                    0x0D  //数据通道3接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define RX_ADDR_P4                                    0x0E  //数据通道4接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define RX_ADDR_P5                                    0x0F  //数据通道5接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define TX_ADDR                                       0x10  //发送地址(低字节在前),ShockBurstTM模式下,RX_ADDR_P0与此地址相等
#define RX_PW_P0                                      0x11  //接收数据通道0有效数据宽度(1~32字节),设置为0则非法
#define RX_PW_P1                                      0x12  //接收数据通道1有效数据宽度(1~32字节),设置为0则非法
#define RX_PW_P2                                      0x13  //接收数据通道2有效数据宽度(1~32字节),设置为0则非法
#define RX_PW_P3                                      0x14  //接收数据通道3有效数据宽度(1~32字节),设置为0则非法
#define RX_PW_P4                                      0x15  //接收数据通道4有效数据宽度(1~32字节),设置为0则非法
#define RX_PW_P5                                      0x16  //接收数据通道5有效数据宽度(1~32字节),设置为0则非法
#define NRF_FIFO_STATUS                               0x17  //FIFO状态寄存器;bit0,RX FIFO寄存器空标志;bit1,RX FIFO满标志;bit2,3,保留
                                                            //bit4,TX FIFO空标志;bit5,TX FIFO满标志;bit6,1,循环发送上一数据包.0,不循环;
// NRF24L01发送接收数据宽度定义
#define TX_ADR_WIDTH                                  5   	//5字节的地址宽度
#define RX_ADR_WIDTH                                  5   	//5字节的地址宽度
#define TX_PLOAD_WIDTH                                32  	//32字节的用户数据宽度
#define RX_PLOAD_WIDTH                                32  	//32字节的用户数据宽度
                     
#define RC_CHANNEL_MIN 1000 	//每个通道的PWM最小值
#define RC_CHANNEL_MAX 2000 	//每个通道的PWM最大值
#define SBUS_MIN_OFFSET 173 	//SBUS协议的最小值
#define SBUS_MID_OFFSET 992 	//SBUS协议的中间值
#define SBUS_MAX_OFFSET 1811 	//SBUS协议的最大值
#define SBUS_CHANNEL_NUMBER 16 	//SBUS通道数，16个模拟通道，2个数字通道
#define SBUS_PACKET_LENGTH 25	// SBUS数据包的字节长度
#define SBUS_FRAME_HEADER 0x0f		// SBUS数据头，开始字节
#define SBUS_FRAME_FOOTER 0x00		// SBUS数据尾，结束字节
#define SBUS_FRAME_FOOTER_V2 0x04	// SBUS数据尾，结束字节2
#define SBUS_STATE_FAILSAFE 0x08	// 激活故障安全标志
#define SBUS_STATE_SIGNALLOSS 0x04 	// 丢失信号标志
#define SBUS_UPDATE_RATE 10 //SBUS输出更新时间ms
#define chNum 8		//采样通道数


/* 函数声明 ------------------------------------------------------------------*/
 
void NRF24L01_Init(void);
void NRF24L01_RX_Mode(void);					//配置为接收模式
void NRF24L01_TX_Mode(void);					//配置为发送模式
uint8_t NRF24L01_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t uint8_ts);//写数据区
uint8_t NRF24L01_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t uint8_ts);	//读数据区		  
uint8_t NRF24L01_Read_Reg(uint8_t reg);					//读寄存器
uint8_t NRF24L01_Write_Reg(uint8_t reg, uint8_t value);		//写寄存器
uint8_t NRF24L01_Check(void);						//检查24L01是否存在
uint8_t NRF24L01_TxPacket(uint8_t *txbuf);				//发送一个包的数据
uint8_t NRF24L01_RxPacket(uint8_t *rxbuf);				//接收一个包的数据
void NRF_LowPower_Mode(void);	
void sbusPreparePacket(uint8_t packet[], uint16_t channels[], uint8_t isSignalLoss, uint8_t isFailsafe);
float map(float value,float fromLow,float fromHigh,float toLow,float toHigh); 
 
#endif
