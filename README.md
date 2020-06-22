# STM32_RC_Transmitter

 基于STM32F103C8T6和MDK开发航模遥控器

## 1.材料清单 
- STM32F103C8T6蓝色开发板1（黑色板也可以） 
![Alt text](https://imgconvert.csdnimg.cn/aHR0cHM6Ly9nZDQuYWxpY2RuLmNvbS9pbWdleHRyYS9pNC8yNjU4NTkyMDE1L1RCMmU1Rk1wQm1XQnVOa1NuZFZYWGNzQXBYYV8hITI2NTg1OTIwMTUuanBn?x-oss-process=image/format,png)
- USB转TTL模块1 
![Alt text](https://img-blog.csdnimg.cn/20200415154643988.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3dlaXhpbl80MjI2ODA1NA==,size_16,color_FFFFFF,t_70)
- ST-LINK V2仿真器下载器1（调试STM32性价比极高）
![Alt text](https://img-blog.csdnimg.cn/20200620224213681.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3dlaXhpbl80MjI2ODA1NA==,size_16,color_FFFFFF,t_70) 
- OLED屏幕（4管脚）1 
![Alt text](https://img-blog.csdnimg.cn/20200210212242113.jpg?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3dlaXhpbl80MjI2ODA1NA==,size_16,color_FFFFFF,t_70)
- 航模遥控器所用的遥杆2 
![Alt text](https://gd4.alicdn.com/imgextra/i1/164533761/O1CN0198NtZF1deaB770qmM_!!164533761.jpg)
- 两段/三段钮子开关>=2 
![Alt text](https://gd1.alicdn.com/imgextra/i1/2658592015/O1CN01D4Hdtr1Qkue5ntaJC_!!2658592015.jpg)
- 3.7V锂电池1 
- 升压充电模块*1

## 2.安装Keil5(MDK) 
- 参考链接进行安装：https://blog.csdn.net/weixin_42911200/article/details/81590158
- 注意要安装Keil.STM32F1xx_DFP.2.3.0.pack支持包，因为我们要用STM32F103C8T6芯片的库函数编写。

## 3.新建工程 
- 新建keil库函数工程，参考链接 https://www.cnblogs.com/zeng-1995/p/11308622.html


