#include "nrf24l01.h"
#include "delay.h"
#include "spi.h"
#include "usart.h"
    
const u8 TX_ADDRESS[TX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //发送地址
const u8 RX_ADDRESS[RX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01};

/*=======================================================
* 函  数：void NRF24L01_Init(void)
* 功  能：NRF24L01初始化
* 参  数：无
* 返回值：无
* 备  注：对应GPIO设置
=======================================================*/
void NRF24L01_Init(void)
{ 	
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);	 //使能PA/B端口时钟
    	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;				 //PB12推挽
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);	//初始化PB12推挽输出-NRF_CSN
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;	
	GPIO_Init(GPIOA, &GPIO_InitStructure);	//初始化PA8推挽输出-NRF_CE
				
  
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_9;   
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA9 中断上拉输入-NRF_IRQ  
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_ResetBits(GPIOA,GPIO_Pin_8|GPIO_Pin_9);//PA8上拉	
	GPIO_ResetBits(GPIOB,GPIO_Pin_12);//PB12上拉	
		 
	SPI2_Init();    		//初始化SPI	 
 
	SPI_Cmd(SPI2, DISABLE); // SPI外设不使能，先关闭SPI，再进行设置

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//SPI主机
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;	//发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		//时钟悬空低0
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	//数据捕获于第1个时钟沿0
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS信号由软件控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;		//定义波特率预分频的值:波特率预分频值为16
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值计算的多项式
	SPI_Init(SPI2, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
 
	SPI_Cmd(SPI2, ENABLE); //使能SPI外设
			 
	NRF24L01_CE=0; 			//使能24L01，在CSN为低的情况下，CE协同NRF24L01的CONFIG寄存器共同决定NRF24L01的状态
	NRF24L01_CSN=1;			//SPI片选取消，低电平时工作 
	 		 	 
}

/*=======================================================
* 函  数：void NRF24L01_Init(void)
* 功  能：外部中断初始化-PA9-IRQ
* 参  数：无
* 返回值：无
* 备  注：GPIO设置，外部中断设置
=======================================================*/
void EXTIX_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;  //上拉输入
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	GPIO_ResetBits(GPIOA,GPIO_Pin_9);//PA9上拉

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource9);     //NRF24L01 IRQ  PA9
	EXTI_InitStructure.EXTI_Line = EXTI_Line9;                       //NRF24L01 IRQ PA9
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;              //EXTI中断
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;          //下降沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;                          
	EXTI_Init(&EXTI_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//使能IRQ在的外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//抢占优先级1 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;			//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure); 
}

/*=======================================================
* 函  数：void EXTI9_5_IRQHandler(void)
* 功  能：NRF24L01接收模式-外部中断服务程序
* 参  数：无
* 返回值：无
* 备  注：接收到数据时，IRQ=0触发外部中断
=======================================================*/
//void EXTI9_5_IRQHandler(void)
//{	
//	u8 istatus;
//	delay_ms(5);
//	if(EXTI_GetITStatus(EXTI_Line9)!=RESET)   //是否发生中断
//	{
//		if(NRF24L01_IRQ==0)
//		{
//			istatus=NRF24L01_Read_Reg(STATUS);            // 读取状态寄存其来判断数据接收状况
//			
//			NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,istatus);//清除状态寄存器
//			if(istatus&0x40)//bit6:数据接收中断
//			{
//				NRF24L01_CE=0;
//				NRF24L01_Read_Buf(RD_RX_PLOAD, chPacket,RX_PLOAD_WIDTH);//读取数据至chPacket
//				NRF24L01_Write_Reg(FLUSH_RX,0xff);//清除RX FIFO寄存器
//				NRF24L01_CE=1;
//				delay_us(150);
//			}
//			else if((istatus&0x10)>0)////达到最大发送次数中断 
//			{
//				NRF24L01_Write_Reg(FLUSH_TX,0xff);//清除TX FIFO寄存器 
//				//NRF24L01_RX_Mode();			//发送结束，转为接收状态。
//				//（可以在中断中转为接收状态，也可在NRF24L01_TxPacket之后转为接收状态）。如果不处理达到最大发送次数的情况下。中断中转换更好一点。
//			}
//			else if((istatus&0x20)>0)//TX发送完成中断
//			{
//				NRF24L01_Write_Reg(FLUSH_TX,0xff);//清除TX FIFO寄存器
//				//NRF24L01_RX_Mode();			//发送结束，转为接收状态。
//			}
//		}
//		NRF24L01_RX_Mode();//接收模式 
//		delay_ms(5);
//		NRF24L01_IRQ=1;
//		EXTI_ClearITPendingBit(EXTI_Line9); //清除LINE9上的中断标志位  
//	}	
//}

/*=======================================================
* 函  数：u8 NRF24L01_Check(void)
* 功  能：检测NRF24L01是否存在
* 参  数：无
* 返回值：0:成功；1:失败
* 备  注：往NRF24L01的发送地址寄存器写入5字节数据再读出来,
		  判断24L01是否已正常工作
=======================================================*/	
u8 NRF24L01_Check(void)
{
	u8 buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
	u8 i;
	SPI2_SetSpeed(SPI_BaudRatePrescaler_4); //spi速度为9Mhz（24L01的最大SPI时钟为10Mhz）   	 
	NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,buf,5);//写入5个字节的地址.
	NRF24L01_Read_Buf(TX_ADDR,buf,5); //读出写入的地址
	for(i=0;i<5;i++)if(buf[i]!=0XA5)break;	 							   
	if(i!=5)return 1;//检测24L01错误	
	return 0;		 //检测到24L01
}	 	 
/*=======================================================
* 函  数：u8 NRF24L01_Write_Reg(u8 reg,u8 value)
* 功  能：向寄存器写入一字节数据
* 参  数：res:寄存器地址;value:写入的值
* 返回值：status:读取的寄存器状态值
=======================================================*/
u8 NRF24L01_Write_Reg(u8 reg,u8 value)
{
	u8 status;	
   	NRF24L01_CSN=0;                 //使能SPI传输
  	status =SPI2_ReadWriteByte(reg);//发送寄存器号 
  	SPI2_ReadWriteByte(value);      //写入寄存器的值
  	NRF24L01_CSN=1;                 //禁止SPI传输	   
  	return(status);       			//返回状态值
}
/*=======================================================
* 函  数：u8 NRF24L01_Read_Reg(u8 reg)
* 功  能：从寄存器读取一字节数据
* 参  数：res:寄存器地址
* 返回值：reg_val:读取的值
=======================================================*/
u8 NRF24L01_Read_Reg(u8 reg)
{
	u8 reg_val;	    
 	NRF24L01_CSN = 0;          //使能SPI传输		
  	SPI2_ReadWriteByte(reg);   //发送寄存器号
  	reg_val=SPI2_ReadWriteByte(0XFF);//读取寄存器内容
  	NRF24L01_CSN = 1;          //禁止SPI传输		    
  	return(reg_val);           //返回状态值
}	
/*=======================================================
* 函  数：u8 NRF24L01_Read_Buf(u8 reg,u8 *pBuf,u8 len)
* 功  能：在指定位置读取一定长度的数据
* 参  数：reg:指定位置;*pBuf:指定数据的首地址;len:数据长度
* 返回值：status:寄存器的状态
=======================================================*/
u8 NRF24L01_Read_Buf(u8 reg,u8 *pBuf,u8 len)
{
	u8 status,u8_ctr;	       
  	NRF24L01_CSN = 0;           //使能SPI传输
  	status=SPI2_ReadWriteByte(reg);//发送寄存器值(位置),并读取状态值   	   
 	for(u8_ctr=0;u8_ctr<len;u8_ctr++)pBuf[u8_ctr]=SPI2_ReadWriteByte(0XFF);//读出数据
  	NRF24L01_CSN=1;       //关闭SPI传输
  	return status;        //返回读到的状态值
}
/*=======================================================
* 函  数：u8 NRF24L01_Write_Buf(u8 reg, u8 *pBuf, u8 len)
* 功  能：向指定位置写入一定长度的数据
* 参  数：reg:指定位置;*pBuf:指定数据的首地址;len:数据长度
* 返回值：status:寄存器的状态
=======================================================*/
u8 NRF24L01_Write_Buf(u8 reg, u8 *pBuf, u8 len)
{
	u8 status,u8_ctr;	    
 	NRF24L01_CSN = 0;          //使能SPI传输
  	status = SPI2_ReadWriteByte(reg);//发送寄存器值(位置),并读取状态值
  	for(u8_ctr=0; u8_ctr<len; u8_ctr++)SPI2_ReadWriteByte(*pBuf++); //写入数据	 
  	NRF24L01_CSN = 1;       //关闭SPI传输
  	return status;          //返回读到的状态值
}	
/*=======================================================
* 函  数：u8 NRF24L01_TxPacket(u8 *txbuf)
* 功  能：NRF24L01发送一次数据
* 参  数：*txbuf：待发送数据的首地址
* 返回值：MAX_TX:最大重发次数;TX_OK:发送完成;0xFF:发送失败
=======================================================*/
u8 NRF24L01_TxPacket(u8 *txbuf)
{
	u8 sta;
 	SPI2_SetSpeed(SPI_BaudRatePrescaler_8);//spi速度为9Mhz（24L01的最大SPI时钟为10Mhz）   
	NRF24L01_CE=0;
  	NRF24L01_Write_Buf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);//写数据到TX BUF  32个字节
 	NRF24L01_CE=1;//启动发送	   
	//while(NRF24L01_IRQ!=0);//等待发送完成
	sta=NRF24L01_Read_Reg(STATUS);  //读取状态寄存器的值	   
	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //清除TX_DS或MAX_RT中断标志
	if(sta&MAX_TX)//达到最大重发次数
	{
		NRF24L01_Write_Reg(FLUSH_TX,0xff);//清除TX FIFO寄存器 
		return MAX_TX; 
	}
	if(sta&TX_OK)//发送完成
	{
		return TX_OK;
	}
	return 0xff;//其他原因发送失败
}
/*=======================================================
* 函  数：u8 NRF24L01_RxPacket(u8 *rxbuf)
* 功  能：NRF24L01接收一次数据
* 参  数：*rxbuf：等待接收数据的首地址
* 返回值：0:接收成功;1:接收数据失败
=======================================================*/
u8 NRF24L01_RxPacket(u8 *rxbuf)
{
	u8 sta;		    							   
	SPI2_SetSpeed(SPI_BaudRatePrescaler_8); //spi速度为9Mhz（24L01的最大SPI时钟为10Mhz）   
	sta=NRF24L01_Read_Reg(STATUS);  //读取状态寄存器的值    	 
	NRF24L01_Write_Reg(NRF_WRITE_REG+STATUS,sta); //清除TX_DS或MAX_RT中断标志
	if(sta&RX_OK)//接收到数据
	{
		NRF24L01_Read_Buf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//读取数据
		NRF24L01_Write_Reg(FLUSH_RX,0xff);//清除RX FIFO寄存器 
		return 0; 
	}	   
	return 1;//没收到任何数据
}	
/*=======================================================
* 函  数：void NRF24L01_RX_Mode(void)
* 功  能：NRF24L01接收模式配置
* 参  数：无
* 返回值：无
* 备  注：设置RX地址,写RX数据宽度,选择RF频道,波特率和LNA HCURR
		  当CE变高后,即进入RX模式,并可以接收数据了
=======================================================*/ 
void NRF24L01_RX_Mode(void)
{
	NRF24L01_CE=0;	  
  	NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(u8*)RX_ADDRESS,RX_ADR_WIDTH);//写RX节点地址
	  
  	NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x01);    //使能通道0的自动应答    
  	NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x01);//使能通道0的接收地址  	 
  	NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,40);	     //设置RF通信频率		  
  	NRF24L01_Write_Reg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//选择通道0的有效数据宽度 	    
  	NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f);//设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
  	NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG, 0x0f);//配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式 
  	NRF24L01_CE = 1; //CE为高,进入接收模式 
}	
/*=======================================================
* 函  数：void NRF24L01_TX_Mode(void)
* 功  能：NRF24L01发射模式配置
* 参  数：无
* 返回值：无
* 备  注：设置TX地址,写TX数据宽度,设置RX自动应答的地址,
		  填充TX发送数据,选择RF频道,波特率和LNA HCURR
		  PWR_UP,CRC使能，CE为高大于10us,则启动发送。
=======================================================*/ 
 
void NRF24L01_TX_Mode(void)
{														 
	NRF24L01_CE=0;	    
  	NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,(u8*)TX_ADDRESS,TX_ADR_WIDTH);//写TX节点地址 
  	NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(u8*)RX_ADDRESS,RX_ADR_WIDTH); //设置TX节点地址,主要为了使能ACK	  

  	NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x01);     //使能通道0的自动应答    
  	NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x01); //使能通道0的接收地址  
  	NRF24L01_Write_Reg(NRF_WRITE_REG+SETUP_RETR,0x1a);//设置自动重发间隔时间:500us + 86us;最大自动重发次数:10次
  	NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,40);       //设置RF通道为40
  	NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f);  //设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
  	NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG,0x0e);    //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式,开启所有中断
	NRF24L01_CE=1;//CE为高,10us后启动发送
}




