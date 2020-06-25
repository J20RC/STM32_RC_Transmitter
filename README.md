# STM32_RC_Transmitter

## 歼20航模遥控器，基于STM32F103C8T6和MDK标准库开发的航模遥控器
 项目计划：https://blog.csdn.net/weixin_42268054/article/details/106955533 
## 开发人员（排名不分先后）：
 * CaiZi负责基本版遥控器；
 * A-XIN负责OLED菜单显示；
 
## 欢迎熟悉STM32开发、AD画图、结构设计、PC端应用程序、Ardroid开发等技术的有识之士加入我们的开源队伍哈~
 - QQ群：1091996634，密码：歼20
 - 个人QQ：2023187082，加好友请备注CSDN

## 1.材料清单 
- STM32F103C8T6蓝色开发板*1【images/bluepill.jpg】
- USB转TTL模块*1 【images/usbttl.png】
- ST-LINK V2仿真器下载器*1（调试STM32性价比极高）【images/stlink.png】
- OLED屏幕（7管脚）*1 【images/OLED.png】
- NRF24L01模块*1 【images/NRF24L01.png】
- 航模遥控器所用的遥杆*2 (如图遥控器15RMB，使用上面的遥杆即可) 【images/yg.png】
- 两段/三段钮子开关>=2 【images/kg.png】
- 3.7V锂电池*1 【images/bat.png】
- 锂电池充电模块*1 【images/cd.png】

## 2.安装Keil5(MDK) 
- 参考链接进行安装：https://blog.csdn.net/weixin_42911200/article/details/81590158
- 注意要安装Keil.STM32F1xx_DFP.2.3.0.pack支持包，因为我们要用STM32F103C8T6芯片的库函数编写。

## 3.新建工程 
- 新建keil库函数工程，参考链接 https://www.cnblogs.com/zeng-1995/p/11308622.html


