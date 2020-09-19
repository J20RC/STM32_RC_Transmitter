#include "nrf24L01.h"
#include "spi.h"
 
const uint8_t TX_ADDRESS[TX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //发送地址
const uint8_t RX_ADDRESS[RX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //接收地址
 
 
//针对NRF24L01修改SPI1驱动
void NRF24L01_SPI_Init(void)
{
    __HAL_SPI_DISABLE(&hspi1);               //先关闭SPI1
    hspi1.Init.CLKPolarity=SPI_POLARITY_LOW; //串行同步时钟的空闲状态为低电平
    hspi1.Init.CLKPhase=SPI_PHASE_1EDGE;     //串行同步时钟的第1个跳变沿（上升或下降）数据被采样
    HAL_SPI_Init(&hspi1);
    __HAL_SPI_ENABLE(&hspi1);                //使能SPI1
}
 
//初始化24L01的IO口
void NRF24L01_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOA_CLK_ENABLE();			//开启GPIOA时钟
//    __HAL_RCC_GPIOB_CLK_ENABLE();			//开启GPIOB时钟
    
     
	//GPIOA4,15推挽输出 = NRF_CE,NRF_CSN
    GPIO_Initure.Pin=GPIO_PIN_4|GPIO_PIN_15;
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //输出
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);     //初始化
	
	//GPIOA.4上拉输入
		GPIO_Initure.Pin=GPIO_PIN_5;				//PA5 -> NRF_IRQ
		GPIO_Initure.Mode=GPIO_MODE_INPUT;      	//输入
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);     	//初始化
			
//		MX_SPI1_Init();    		              			//初始化SPI1
		NRF24L01_SPI_Init();                			//针对NRF的特点修改SPI的设置
		NRF24L01_CE_LOW(); 			            			//使能24L01
		NRF24L01_SPI_CS_DISABLE();			    			//SPI片选取消	 		 	 
}
 
/**
	*SPI速度设置函数
  *SPI速度=fAPB1/分频系数
  *@ref SPI_BaudRate_Prescaler:SPI_BAUDRATEPRESCALER_2~SPI_BAUDRATEPRESCALER_2 256
	*fAPB1时钟一般为42Mhz：
	*/
static void SPI1_SetSpeed(uint8_t SPI_BaudRatePrescaler)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//判断有效性
    __HAL_SPI_DISABLE(&hspi1);            //关闭SPI
    hspi1.Instance->CR1&=0XFFC7;          //位3-5清零，用来设置波特率
    hspi1.Instance->CR1|=SPI_BaudRatePrescaler;//设置SPI速度
    __HAL_SPI_ENABLE(&hspi1);             //使能SPI
}
 
/**
  * 函数功能: 往串行Flash读取写入一个字节数据并接收一个字节数据
  * 输入参数: byte：待发送数据
  * 返 回 值: uint8_t：接收到的数据
  * 说    明：无
  */
uint8_t SPIx_ReadWriteByte(SPI_HandleTypeDef* hspi,uint8_t byte)
{
  uint8_t d_read,d_send=byte;
  if(HAL_SPI_TransmitReceive(hspi,&d_send,&d_read,1,0xFF)!=HAL_OK)
  {
    d_read=0xFF;
  }
  return d_read; 
}
 
/**
  * 函数功能: 检测24L01是否存在
  * 输入参数: 无
  * 返 回 值: 0，成功;1，失败
  * 说    明：无          
  */ 
uint8_t NRF24L01_Check(void)
{
	uint8_t buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
	uint8_t i;
	SPI1_SetSpeed(SPI_BAUDRATEPRESCALER_4); //spi速度为8.0Mhz（（24L01的最大SPI时钟为10Mhz,这里大一点没关系）  
	NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,buf,5);//写入5个字节的地址.	
	NRF24L01_Read_Buf(TX_ADDR,buf,5); //读出写入的地址  
	for(i=0;i<5;i++)if(buf[i]!=0XA5)break;	 							   
	if(i!=5)return 1;//检测24L01错误	
	return 0;		 	//检测到24L01
}	
 
/**
  * 函数功能: SPI写寄存器
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：reg:指定寄存器地址
  *           
  */ 
uint8_t NRF24L01_Write_Reg(uint8_t reg,uint8_t value)
{
	uint8_t status;	
  NRF24L01_SPI_CS_ENABLE();                 //使能SPI传输
  status =SPIx_ReadWriteByte(&hspi1,reg);   //发送寄存器号 
  SPIx_ReadWriteByte(&hspi1,value);         //写入寄存器的值
  NRF24L01_SPI_CS_DISABLE();                //禁止SPI传输	   
  return(status);       			//返回状态值
}
 
/**
  * 函数功能: 读取SPI寄存器值
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：reg:要读的寄存器
  *           
  */ 
uint8_t NRF24L01_Read_Reg(uint8_t reg)
{
	uint8_t reg_val;	    
 	NRF24L01_SPI_CS_ENABLE();          //使能SPI传输		
  SPIx_ReadWriteByte(&hspi1,reg);   //发送寄存器号
  reg_val=SPIx_ReadWriteByte(&hspi1,0XFF);//读取寄存器内容
  NRF24L01_SPI_CS_DISABLE();          //禁止SPI传输		    
  return(reg_val);           //返回状态值
}		
 
/**
  * 函数功能: 在指定位置读出指定长度的数据
  * 输入参数: 无
  * 返 回 值: 此次读到的状态寄存器值 
  * 说    明：无
  *           
  */ 
uint8_t NRF24L01_Read_Buf(uint8_t reg,uint8_t *pBuf,uint8_t len)
{
	uint8_t status,uint8_t_ctr;	   
  
  NRF24L01_SPI_CS_ENABLE();           //使能SPI传输
  status=SPIx_ReadWriteByte(&hspi1,reg);//发送寄存器值(位置),并读取状态值   	   
 	for(uint8_t_ctr=0;uint8_t_ctr<len;uint8_t_ctr++)
  {
    pBuf[uint8_t_ctr]=SPIx_ReadWriteByte(&hspi1,0XFF);//读出数据
  }
  NRF24L01_SPI_CS_DISABLE();       //关闭SPI传输
  return status;        //返回读到的状态值
}
 
/**
  * 函数功能: 在指定位置写指定长度的数据
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：reg:寄存器(位置)  *pBuf:数据指针  len:数据长度
  *           
  */ 
uint8_t NRF24L01_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t len)
{
	uint8_t status,uint8_t_ctr;	    
 	NRF24L01_SPI_CS_ENABLE();          //使能SPI传输
  status = SPIx_ReadWriteByte(&hspi1,reg);//发送寄存器值(位置),并读取状态值
  for(uint8_t_ctr=0; uint8_t_ctr<len; uint8_t_ctr++)
  {
    SPIx_ReadWriteByte(&hspi1,*pBuf++); //写入数据	 
  }
  NRF24L01_SPI_CS_DISABLE();       //关闭SPI传输
  return status;          //返回读到的状态值
}		
 
/**
  * 函数功能: 启动NRF24L01发送一次数据
  * 输入参数: 无
  * 返 回 值: 发送完成状况
  * 说    明：txbuf:待发送数据首地址
  *           
  */ 
uint8_t NRF24L01_TxPacket(uint8_t *txbuf)
{
	uint8_t sta;
	SPI1_SetSpeed(SPI_BAUDRATEPRESCALER_8); //spi速度为4.0Mhz（24L01的最大SPI时钟为10Mhz） 
	NRF24L01_CE_LOW();
  NRF24L01_Write_Buf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);//写数据到TX BUF  32个字节
 	NRF24L01_CE_HIGH();//启动发送	 
  
	while(NRF24L01_IRQ_PIN_READ()!=0);//等待发送完成
  
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
 
/**
  * 函数功能:启动NRF24L01接收一次数据
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  *           
  */ 
uint8_t NRF24L01_RxPacket(uint8_t *rxbuf)
{
	uint8_t sta;		
  SPI1_SetSpeed(SPI_BAUDRATEPRESCALER_8); //spi速度为4.0Mhz（24L01的最大SPI时钟为10Mhz） 
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
 
/**
  * 函数功能: 该函数初始化NRF24L01到RX模式
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  *           
  */ 
void NRF24L01_RX_Mode(void)
{
	NRF24L01_CE_LOW();	  
  NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG, 0x0F);//配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC 
  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x01);    //使能通道0的自动应答    
  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x01);//使能通道0的接收地址  	 
  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,40);	     //设置RF通信频率		  
  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f);//设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
  
  NRF24L01_Write_Reg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//选择通道0的有效数据宽度 	    
    
  NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(uint8_t*)RX_ADDRESS,RX_ADR_WIDTH);//写RX节点地址
	
  NRF24L01_CE_HIGH(); //CE为高,进入接收模式 
  HAL_Delay(1);
}	
 
/**
  * 函数功能: 该函数初始化NRF24L01到TX模式
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  *           
  */ 
void NRF24L01_TX_Mode(void)
{														 
	NRF24L01_CE_LOW();	    
  NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,(uint8_t*)TX_ADDRESS,TX_ADR_WIDTH);//写TX节点地址 
  NRF24L01_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,(uint8_t*)RX_ADDRESS,RX_ADR_WIDTH); //设置TX节点地址,主要为了使能ACK	  
 
  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_AA,0x01);     //使能通道0的自动应答    
  NRF24L01_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x01); //使能通道0的接收地址  
  NRF24L01_Write_Reg(NRF_WRITE_REG+SETUP_RETR,0xff);//设置自动重发间隔时间:4000us + 86us;最大自动重发次数:15次
  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_CH,40);       //设置RF通道为40
  NRF24L01_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f);  //设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
  NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG,0x0e);    //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式,开启所有中断
	NRF24L01_CE_HIGH();//CE为高,10us后启动发送
  HAL_Delay(1);
}
 
/**
  * 函数功能: 该函数NRF24L01进入低功耗模式
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  *           
  */
void NRF_LowPower_Mode(void)
{
	NRF24L01_CE_LOW();	 
	NRF24L01_Write_Reg(NRF_WRITE_REG+CONFIG, 0x00);		//配置工作模式:掉电模式
}

//SBUS数据包生成函数
void sbusPreparePacket(uint8_t packet[], uint16_t channels[], uint8_t isSignalLoss, uint8_t isFailsafe)
{
    static int output[SBUS_CHANNEL_NUMBER] = {0};//这里一定要16个元素的数组，不然其他通道会干扰
    /*将channel值1000-2000映射到SBUS协议的173-1811*/
    for (uint8_t i = 0; i < SBUS_CHANNEL_NUMBER; i++) {
        output[i] = (int)map(channels[i], RC_CHANNEL_MIN, RC_CHANNEL_MAX, SBUS_MIN_OFFSET, SBUS_MAX_OFFSET);
    }
    
    uint8_t stateByte = 0x00;
    if (isSignalLoss) {
        stateByte |= SBUS_STATE_SIGNALLOSS; // 丢失信号标志
    }
    if (isFailsafe) {
        stateByte |= SBUS_STATE_FAILSAFE;   // 激活故障安全标志
    }
    packet[0] = SBUS_FRAME_HEADER; //SBUS头，0x0F
    packet[1] = (uint8_t) (output[0] & 0x07FF);
    packet[2] = (uint8_t) ((output[0] & 0x07FF)>>8 | (output[1] & 0x07FF)<<3);
    packet[3] = (uint8_t) ((output[1] & 0x07FF)>>5 | (output[2] & 0x07FF)<<6);
    packet[4] = (uint8_t) ((output[2] & 0x07FF)>>2);
    packet[5] = (uint8_t) ((output[2] & 0x07FF)>>10 | (output[3] & 0x07FF)<<1);
    packet[6] = (uint8_t) ((output[3] & 0x07FF)>>7 | (output[4] & 0x07FF)<<4);
    packet[7] = (uint8_t) ((output[4] & 0x07FF)>>4 | (output[5] & 0x07FF)<<7);
    packet[8] = (uint8_t) ((output[5] & 0x07FF)>>1);
    packet[9] = (uint8_t) ((output[5] & 0x07FF)>>9 | (output[6] & 0x07FF)<<2);
    packet[10] = (uint8_t) ((output[6] & 0x07FF)>>6 | (output[7] & 0x07FF)<<5);
    packet[11] = (uint8_t) ((output[7] & 0x07FF)>>3);
    packet[12] = (uint8_t) ((output[8] & 0x07FF));
    packet[13] = (uint8_t) ((output[8] & 0x07FF)>>8 | (output[9] & 0x07FF)<<3);
    packet[14] = (uint8_t) ((output[9] & 0x07FF)>>5 | (output[10] & 0x07FF)<<6);  
    packet[15] = (uint8_t) ((output[10] & 0x07FF)>>2);
    packet[16] = (uint8_t) ((output[10] & 0x07FF)>>10 | (output[11] & 0x07FF)<<1);
    packet[17] = (uint8_t) ((output[11] & 0x07FF)>>7 | (output[12] & 0x07FF)<<4);
    packet[18] = (uint8_t) ((output[12] & 0x07FF)>>4 | (output[13] & 0x07FF)<<7);
    packet[19] = (uint8_t) ((output[13] & 0x07FF)>>1);
    packet[20] = (uint8_t) ((output[13] & 0x07FF)>>9 | (output[14] & 0x07FF)<<2);
    packet[21] = (uint8_t) ((output[14] & 0x07FF)>>6 | (output[15] & 0x07FF)<<5);
    packet[22] = (uint8_t) ((output[15] & 0x07FF)>>3);
    packet[23] = stateByte;         // 标志位0x00
    packet[24] = SBUS_FRAME_FOOTER; // SBUS结束字节0x00
}


/*函数：float map(float value,float fromLow,float fromHigh,float toLow,float toHigh)
* 说明：仿Arduino,将一个数字从一个范围重新映射到另一个范围
		也就是说，fromLow的值将映射到toLow，fromlhigh到toHigh的值等等。
* 参数：value：待映射的数值；
		fromLow：原范围的最小值
		fromHigh：原范围的最大值
		toLow：要转换的范围的最小值
		toHigh：要转换的范围的最大值
* 返回：转换后的数值
*/
float map(float value,float fromLow,float fromHigh,float toLow,float toHigh)
{
	return ((value-fromLow)*(toHigh-toLow)/(fromHigh-fromLow)+toLow);
}



