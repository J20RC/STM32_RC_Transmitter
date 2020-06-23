#include "config.h"
#include "delay.h"
#include "usart.h"
#include "sys.h"
#include "rtc.h" 
volatile u16 ch1Value[10];//ADC采样值
volatile u16 PWM1value;//控制PWM占空比
#define ADC1_DR_Address    ((u32)0x4001244C)		//ADC1的地址
//通用定时器2中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器2控制ADC1定时采样
void TIM2_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 		//时钟使能

	//定时器TIM2初始化
	TIM_TimeBaseStructure.TIM_Period = arr; 		//设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 			//设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 		//设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 		//TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);			//根据指定的参数初始化TIMx的时间基数单位
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;		//选择定时器模式:TIM脉冲宽度调制模式1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;		//比较输出使能
	TIM_OCInitStructure.TIM_Pulse = 9;							//计数达到9产生中断
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;		//输出极性:TIM输出比较极性低
	TIM_OC2Init(TIM2, & TIM_OCInitStructure);		//初始化外设TIM2_CH2
	
	TIM_Cmd(TIM2, ENABLE); 			//使能TIMx
	TIM_CtrlPWMOutputs(TIM2, ENABLE); 
}


//DMA1配置
void DMA1_Init(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);	  			//使能ADC1通道时钟
	
	//DMA1初始化
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;				//ADC1地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ch1Value; 		//ch1Value的内存地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; 				//方向(从外设到内存)
	DMA_InitStructure.DMA_BufferSize = 10; 						//DMA缓存大小，存放10次采样值
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 	//外设地址固定，接收一次数据后，设备地址禁止后移
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 			//内存地址不固定，接收多次数据后，目标内存地址后移
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord ; //外设数据单位，定义外设数据宽度为16位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord ;    //内存数据单位，HalfWord就是为16位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular  ; 		//DMA模式：循环传输
	DMA_InitStructure.DMA_Priority = DMA_Priority_High ; 	//DMA优先级：高
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;   		//禁止内存到内存的传输
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);  //配置DMA1
	
	DMA_ITConfig(DMA1_Channel1,DMA_IT_TC, ENABLE);		//使能传输完成中断

	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	DMA_Cmd(DMA1_Channel1,ENABLE);
}

//中断处理函数
void  DMA1_Channel1_IRQHandler(void)
{
	int ch1Result=0;
	if(DMA_GetITStatus(DMA1_IT_TC1)!=RESET){
		//中断处理代码
		ch1Result = GetMedianNum(ch1Value,10);//中值滤波
		PWM1value = (int)map(ch1Result,0,4092,1000,2000);//数值映射
		printf("采样值：%d\t",PWM1value);
		printf("当前时间：%d:%d:%d\r\n",calendar.hour,calendar.min,calendar.sec);
		TIM_SetCompare2(TIM3,PWM1value);//输出给PWM
		DMA_ClearITPendingBit(DMA1_IT_TC1);//清除标志
	}
}
//GPIO配置，PA0
void GPIOA_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	  //使能GPIOA时钟

	//PA6 作为模拟通道输入引脚   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);  
}

//初始化ADC
//这里我们仅以规则通道为例
//我们默认将开启通道0~3																	   
void  Adc_Init(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);	  //使能ADC1通道时钟

	//ADC1初始化
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; 			//独立ADC模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;  				//关闭扫描方式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;			//关闭连续转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_CC2;   	//使用外部触发模式
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; 			//采集数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1; 			//要转换的通道数目
	ADC_Init(ADC1, &ADC_InitStructure);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);				//配置ADC时钟，为PCLK2的6分频，即12MHz
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5);		//配置ADC1通道0为239.5个采样周期 
	
	//使能ADC、DMA
	ADC_DMACmd(ADC1,ENABLE);
	ADC_Cmd(ADC1,ENABLE);
 
	ADC_ResetCalibration(ADC1);				//复位校准寄存器
	while(ADC_GetResetCalibrationStatus(ADC1));				//等待校准寄存器复位完成
 
	ADC_StartCalibration(ADC1);				//ADC校准
	while(ADC_GetCalibrationStatus(ADC1));				//等待校准完成
	
	ADC_ExternalTrigConvCmd(ADC1, ENABLE);		//设置外部触发模式使能
}

//获得ADC值
//ch:通道值 0~9
u16 Get_Adc(u8 ch)   
{
  	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADC通道,采样时间为239.5个周期	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能	
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束
	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}

//ch:通道值 0~9，采样times次后作均值滤波
u16 Get_Adc_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
		delay_ms(5);
	}
	return temp_val/times;
}
//TIM3 PWM部分初始化 
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM3_PWM_Init(u16 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//使能定时器3时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);  //使能GPIO外设和AFIO复用功能模块时钟
	
	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); //Timer3部分重映射  TIM3_CH2->PB5    
 
   //设置该引脚为复用输出功能,输出TIM3 CH2的PWM脉冲波形	GPIOB.5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; //TIM_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB
 
   //初始化TIM3
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	//初始化TIM3 Channel2 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式1，计数值<自动重装载值时，输出高电平
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM3 OC2

	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);  //使能TIM3在CCR2上的预装载寄存器

	TIM_Cmd(TIM3, ENABLE);  //使能TIM3
}


/*函数说明：仿Arduino,将一个数字从一个范围重新映射到另一个范围
也就是说，fromLow的值将映射到toLow，fromlhigh到toHigh的值等等。
*/
float map(float value,float fromLow,float fromHigh,float toLow,float toHigh)
{
	return ((value-fromLow)*(toHigh-toLow)/(fromHigh-fromLow)+toLow);
}

/*函数说明：对数组进行中值滤波，返回中值
bArray - 待滤波的数组；iFilterLen - 数组元素个数
*/
int GetMedianNum(volatile u16 * bArray, int iFilterLen)
{
    int i,j;// 循环变量
    int bTemp;

    // 用冒泡法对数组进行排序
    for (j = 0; j < iFilterLen - 1; j ++)
    {
        for (i = 0; i < iFilterLen - j - 1; i ++)
        {
            if (bArray[i] > bArray[i + 1])
            {
                // 互换
                bTemp = bArray[i];
                bArray[i] = bArray[i + 1];
                bArray[i + 1] = bTemp;
            }
        }
    }

    // 计算中值
    if ((iFilterLen & 1) > 0)
    {
        // 数组有奇数个元素，返回中间一个元素
        bTemp = bArray[(iFilterLen + 1) / 2];
    }
    else
    {
        // 数组有偶数个元素，返回中间两个元素平均值
        bTemp = (bArray[iFilterLen / 2] + bArray[iFilterLen / 2 + 1]) / 2;
    }

    return bTemp;
}
