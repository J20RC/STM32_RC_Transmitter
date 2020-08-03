![Alt text](./images/logo.png)
# :airplane: J20航模遥控器
 - 基于STM32和MDK标准库开发的航模遥控器
 - 项目计划：https://blog.csdn.net/weixin_42268054/article/details/106955533 
 - Bilibili项目宣传视频：https://www.bilibili.com/video/BV1Vg4y1v7oQ
 - 我们的开源宗旨：自由 协调 开放 合作 共享
 - 和一群志同道合的人，做自己所热爱的事！
## 拥抱开源，我们期待你的加入！
 - 由于笔者学业繁忙，时间有限，欢迎熟悉STM32开发、PCB设计、结构设计、UI设计、PC端应用程序、Ardroid开发等技术的有识之士加入我们的开源队伍哈~
 - QQ群：1091996634，密码：J20
 - 有意参与本项目开发、维护的模友，加群后请私聊群主哦~

## 1.材料清单 
| 器件名称        			| 数量   	| 上传图片名称  			| 备注 |
| ------------- 			| :------:	| :-------------:			|------|
| STM32F103C8T6最小系统板 	| 2 		| images/stm32f103c8t6.png 	|
| USB转TTL模块			 	| 1 		| images/usbttl(1/2).png 	|
| ST-LINK V2仿真器下载器 	| 1 		| images/stlink.png 		|调试STM32性价比极高|
| OLED屏幕（4管脚）		 	| 1 		| images/OLED.png	 		|
| NRF24L01模块			 	| 2 		| images/nrf24l01(1/2/3).png|
| 航模遥控器所用的遥杆	 	| 2 		| images/yg(ykq).png		|该遥控器15RMB，使用上面的遥杆即可|
| 两段/三段钮子开关		 	| >=2 		| images/nzkg.png		 	|
| 50k电位器				 	| >=2 		| images/dwq.png			|
| 100uF电解电容			 	| 2 		| images/100uf.png	 		|
| 0.1uF独石电容			 	| 2 		| images/104.png	 		|
| 金属膜电阻（1k、10k、20k）| 3 		| images/(1k/10k/20k).png	|
| 轻触开关（键帽）			| 8 		| images/qckg(m).png	 	|
| SS8050三极管			 	| 2 		| images/ss8050.png	 		|
| 洞洞板（7*9cm）			| 2 		| images/ddb.png	 		|
| XH2.54mm插针插座			| n 		| images/cz(1).png	 		|
| L7805CDT稳压芯片			| 1 		| images/l7805.png	 		|
| AMS1117-3.3稳压芯片		| 2 		| images/ams1117-3.3.png	|
| 旋转编码器EC11			| 1 		| images/bmq.png	 		|
| 肖特基二极管1N5819		| 1 		| images/1n5819.png	 		|
| 三脚拨动开关 				| 1 		| images/3jkg.png	 		|
| 3.7V锂电池			 	| 1 		| images/bat.png	 		|
| 锂电池充电模块			| 1 		| images/cd.png		 		|


## 2.安装Keil5(MDK) 
- 参考链接进行安装：https://blog.csdn.net/weixin_42911200/article/details/81590158
- 注意要安装Keil.STM32F1xx_DFP.2.3.0.pack支持包，因为我们要用STM32F103系列芯片的库函数编写。

## 3.新建工程 
- 新建keil库函数工程，参考链接 https://www.cnblogs.com/zeng-1995/p/11308622.html

## 4.代码说明
- 多按键检测(基于有限状态机)：https://blog.csdn.net/weixin_42268054/article/details/107393669
- 多级菜单(树形拓扑结构、状态机)：https://blog.csdn.net/weixin_42268054/article/details/107613192

## 5.版本说明
- 基础版V1.0发布（2020.08.01）	<br/>
实现了8通道遥控和接收，遥控器菜单包含通道微调、通道正反、行程校准功能；	<br/>
制作教程：https://blog.csdn.net/weixin_42268054/article/details/107699434	<br/>
使用说明：https://blog.csdn.net/weixin_42268054/article/details/107709928	<br/>

## 非常感谢项目中所有的贡献者：
 * 蔡子CaiZi（B站up主）@Cai-Zi  https://cai-zi.github.io/
 * A-XIN;
 * 小何;
 * ZL_Studio（B站up主）;
 * 王天天@WangTiantian139  https://wangtiantian139.github.io/
 * 刘桶桶;
 * 简繁;
 * 王博;
 * 与非门;
 * Talons;
 * 大米粥;
 * P a S S ||;


