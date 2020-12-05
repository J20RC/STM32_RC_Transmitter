将key.c和key.h放入config文件夹，即可实现两个版本之间的移植
===================EC11旋转编码器版本===================
六个按键：
1	CH1Left	接PB5	【home】
2	CH1Right	接PB4
3	CH2Down接PB3	【确定】
4	CH2Up	接PA15	【返回】
5	CH4Left	接PA12	【数值-】
6	CH4Right	接PA11	【数值+】
7 旋转编码器模块：
	GND	电源地
	VCC	接3.3V电源
	SW	接PB11
	DT	接PB10
	CLK	接PB1
短按旋转编码器进入菜单，长按旋转编码器/短按CH1Left键退出菜单。

按键布局建议：
                                    4
                                    |
                                    3                        
5-------6                           1-------2
                 
	----------------
             4 |        屏幕        |  7
	----------------

========================按键版本========================
七个按键定义：
1	CH1Left	接PB5	【确定】
2	CH1Right	接PB4	【返回】
3	CH2Down接PB3	【菜单向下】
4	CH2Up	接PA15	【菜单向上】
5	CH4Left	接PA12	【数值-】
6	CH4Right	接PA11	【数值+】
7	MENU	接PB11	【菜单】
短按MENU键进入/退出菜单，CH1Left键和CH1Right键轮询菜单。

按键布局建议：

       4                  1
5---+---6           2
       3                  7

NRF24L01模块：
	GND	电源地
	VCC	接3.3V电源
	CSN	接PB12
	SCK	接PB13
	MISO	接PB14
	MOSI	接PB15
	CE	接PA8
	IRQ	接PA9
ADC采样：PA0-7
电池电压检测：PB0
蜂鸣器：PA10
OLED显示屏：
	GND   	电源地
	VCC   	接3.3V电源
	SCL   	接PB8（SCL）
	SDA   	接PB9（SDA）
串口USB-TTL接法：	
	GND   	电源地
	3V3   	接3.3V
	TXD   	接PB7
	RXD   	接PB6
ST-LINK V2接法：
	GND   	电源地
	3V3   	接3.3V
	SWCLK 	接DCLK
	SWDIO 	接DIO
