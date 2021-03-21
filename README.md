![J20RC](./images/logo.png)<br>
# J20RC BASE
[![Latest version](https://img.shields.io/github/v/release/J20RC/STM32_RC_Transmitter)](https://github.com/J20RC/STM32_RC_Transmitter/releases)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)<br>
强烈推荐Star(￣▽￣)我们开源团队会持续优化整个项目喔~<br>
项目使用STM32标准库/HAL库来开发一套通用的航模遥控器和接收机，软硬件完全开源，还附带教程哦~

* J20RC PRO遥控器<br>

  Github：[开源仓库](https://github.com/J20RC/J20RC_PRO)<br>
  Gitee：[开源仓库](https://gitee.com/J20RC//J20RC_PRO)<br>

- STM32F0接收机<br>
Github：[开源仓库](https://github.com/J20RC/STM32F0_Receiver)<br>
Gitee：[开源仓库](https://gitee.com/J20RC/STM32F0_Receiver)<br>
- STM32F1接收机<br>
Github：[开源仓库](https://github.com/J20RC/STM32F1_Receiver)<br>
Gitee：[开源仓库](https://gitee.com/J20RC/STM32F1_Receiver)<br>
## 硬件 
* V1(2020.12.12)：[下载](./hardware/J20RC_BASE_V1.0_20201212_Schematic.pdf)

* V2(2020.10.21)：[下载](./hardware/J20RC_BASE_V2.0_20201021_PCB_MINI.zip)

  MINI版，适配所有遥控器外壳

* V3(2021.03.11)：[下载](./hardware/J20RC_BASE_V3.2_20210311_PCB(适配15元星球大战遥控器).zip)

  适配星球大战遥控器外壳（15元）


## STM32教程
- 安装Keil5(MDK) ：[博客](https://blog.csdn.net/weixin_42911200/article/details/81590158)

  注意要安装Keil.STM32F1xx_DFP.2.3.0.pack支持包，因为我们要用STM32F103系列芯片的库函数编写。

- 新建keil库函数工程：[博客](https://www.cnblogs.com/zeng-1995/p/11308622.html) 

- 多按键检测(基于有限状态机)：[博客](https://blog.csdn.net/weixin_42268054/article/details/107393669)

- 多级菜单(树形拓扑结构、状态机)：[博客](https://blog.csdn.net/weixin_42268054/article/details/107613192)

- STM32系列教学：[正点原子]() [野火]()

## 制作教程

- 1.制作教程：[博客](https://www.cnblogs.com/cai-zi/p/13438285.html)

- 2.使用说明：[博客](https://www.cnblogs.com/cai-zi/p/13438406.html)

- 3.开发说明：[博客](https://www.cnblogs.com/cai-zi/p/13536862.html)

- 4.PCB打印：[博客](https://www.cnblogs.com/cai-zi/p/13536693.html)

- 5.接收机制作：[博客](https://www.cnblogs.com/cai-zi/p/13693930.html)

- 6.外壳制作：[博客](https://www.cnblogs.com/cai-zi/p/14124682.html)

- 7.PPM输出接口：[博客](https://www.cnblogs.com/cai-zi/p/14167013.html)

## 版本说明
- 基础版1.0发布（2020.08.01）[下载](https://github.com/J20RC/STM32_RC_Transmitter/releases/tag/1.0)	<br>
	实现了8通道遥控和接收，遥控器菜单包含通道微调、通道正反、行程校准功能；	<br>
	[视频](https://www.bilibili.com/video/BV1ea4y1J7WV)	<br/>
	
- 基础版1.1发布（2020.09.25）[下载](https://github.com/J20RC/STM32_RC_Transmitter/releases/tag/1.1)	<br>
	遥控器端系统设置增加左右手油门设置、报警电压、校准电压、微调单位等；	<br>
	遥控器的mini板PCB修复电压供电波动问题，使用AMS1117-3.3芯片替代原来的H7233芯片；	<br>
	
- 基础版2.0发布（2020.10.25）[下载](https://github.com/J20RC/STM32_RC_Transmitter/releases/tag/2.1)<br>
  全新的菜单界面，更丰富的设置功能。<br>
  [视频](https://www.bilibili.com/video/BV1wy4y1k718)

## 问题讨论&技术支持

 - QQ群：1091996634，密码：J20

## 非常感谢项目中所有的贡献者：
 * 蔡子CaiZi（B站up主）@[Cai-Zi](https://cai-zi.github.io/) 
 * A-XIN;
 * 小何;
 * ZL_Studio（B站up主）;
 * 王天天@[WangTiantian139](https://wangtiantian139.github.io/)
 * 刘桶桶;
 * 简繁;
 * 王博;
 * 与非门;
 * Talons;
 * 大米粥（B站up主）;
 * P a S S ||;
 * 佚名;
 * 小吴;
 * xuech007;
 * 欣宏;


