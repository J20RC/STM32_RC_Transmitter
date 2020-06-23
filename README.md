# STM32_RC_Transmitter

 基于STM32F103C8T6和MDK开发航模遥控器

## 1.材料清单 
- STM32F103C8T6蓝色开发板1（黑色板也可以） 
![Alt text](https://github.com/Cai-Zi/STM32_RC_Transmitter/blob/master/images/bluepill.jpg)
- USB转TTL模块1 
![Alt text](https://github.com/Cai-Zi/STM32_RC_Transmitter/blob/master/images/usbttl.png)
- ST-LINK V2仿真器下载器1（调试STM32性价比极高）
![Alt text](https://github.com/Cai-Zi/STM32_RC_Transmitter/blob/master/images/stlink.png)
- OLED屏幕（4管脚）1 
![Alt text](https://github.com/Cai-Zi/STM32_RC_Transmitter/blob/master/images/oled.png)
- NRF24L01模块
![Alt text](https://github.com/Cai-Zi/STM32_RC_Transmitter/blob/master/images/NRF24L01.png)
- 航模遥控器所用的遥杆2 (如图遥控器15RMB，使用上面的遥杆即可)
![Alt text](https://github.com/Cai-Zi/STM32_RC_Transmitter/blob/master/images/yg.png)
- 两段/三段钮子开关>=2 
![Alt text](https://github.com/Cai-Zi/STM32_RC_Transmitter/blob/master/images/kg.png)
- 3.7V锂电池1 
![Alt text](https://github.com/Cai-Zi/STM32_RC_Transmitter/blob/master/images/bat.png)
- 锂电池充电模块*1
![Alt text](https://github.com/Cai-Zi/STM32_RC_Transmitter/blob/master/images/cd.png)

## 2.安装Keil5(MDK) 
- 参考链接进行安装：https://blog.csdn.net/weixin_42911200/article/details/81590158
- 注意要安装Keil.STM32F1xx_DFP.2.3.0.pack支持包，因为我们要用STM32F103C8T6芯片的库函数编写。

## 3.新建工程 
- 新建keil库函数工程，参考链接 https://www.cnblogs.com/zeng-1995/p/11308622.html


