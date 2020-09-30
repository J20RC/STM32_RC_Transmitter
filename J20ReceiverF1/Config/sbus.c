#include "main.h"
#include "sbus.h"
#include "delay.h"
#include "usart.h"
#include "sys.h"

void sbusPreparePacket(u8 packet[], u16 channels[], u8 isSignalLoss, u8 isFailsafe)
{
    static int output[SBUS_CHANNEL_NUMBER] = {0};//这里一定要16个元素的数组，不然其他通道会干扰
    /*将channel值1000-2000映射到SBUS协议的173-1811*/
    for (u8 i = 0; i < SBUS_CHANNEL_NUMBER; i++) {
        output[i] = (int)map(channels[i], RC_CHANNEL_MIN, RC_CHANNEL_MAX, SBUS_MIN_OFFSET, SBUS_MAX_OFFSET);
    }
    
    u8 stateByte = 0x00;
    if (isSignalLoss) {
        stateByte |= SBUS_STATE_SIGNALLOSS; // 丢失信号标志
    }
    if (isFailsafe) {
        stateByte |= SBUS_STATE_FAILSAFE;   // 激活故障安全标志
    }
    packet[0] = SBUS_FRAME_HEADER; //SBUS头，0x0F
    packet[1] = (u8) (output[0] & 0x07FF);
    packet[2] = (u8) ((output[0] & 0x07FF)>>8 | (output[1] & 0x07FF)<<3);
    packet[3] = (u8) ((output[1] & 0x07FF)>>5 | (output[2] & 0x07FF)<<6);
    packet[4] = (u8) ((output[2] & 0x07FF)>>2);
    packet[5] = (u8) ((output[2] & 0x07FF)>>10 | (output[3] & 0x07FF)<<1);
    packet[6] = (u8) ((output[3] & 0x07FF)>>7 | (output[4] & 0x07FF)<<4);
    packet[7] = (u8) ((output[4] & 0x07FF)>>4 | (output[5] & 0x07FF)<<7);
    packet[8] = (u8) ((output[5] & 0x07FF)>>1);
    packet[9] = (u8) ((output[5] & 0x07FF)>>9 | (output[6] & 0x07FF)<<2);
    packet[10] = (u8) ((output[6] & 0x07FF)>>6 | (output[7] & 0x07FF)<<5);
    packet[11] = (u8) ((output[7] & 0x07FF)>>3);
    packet[12] = (u8) ((output[8] & 0x07FF));
    packet[13] = (u8) ((output[8] & 0x07FF)>>8 | (output[9] & 0x07FF)<<3);
    packet[14] = (u8) ((output[9] & 0x07FF)>>5 | (output[10] & 0x07FF)<<6);  
    packet[15] = (u8) ((output[10] & 0x07FF)>>2);
    packet[16] = (u8) ((output[10] & 0x07FF)>>10 | (output[11] & 0x07FF)<<1);
    packet[17] = (u8) ((output[11] & 0x07FF)>>7 | (output[12] & 0x07FF)<<4);
    packet[18] = (u8) ((output[12] & 0x07FF)>>4 | (output[13] & 0x07FF)<<7);
    packet[19] = (u8) ((output[13] & 0x07FF)>>1);
    packet[20] = (u8) ((output[13] & 0x07FF)>>9 | (output[14] & 0x07FF)<<2);
    packet[21] = (u8) ((output[14] & 0x07FF)>>6 | (output[15] & 0x07FF)<<5);
    packet[22] = (u8) ((output[15] & 0x07FF)>>3);
    packet[23] = stateByte;         // 标志位0x00
    packet[24] = SBUS_FRAME_FOOTER; // SBUS结束字节0x00
}
void sbusData(u8 SBUS_DATA[], u16 CH_Rec[])
{
	u32 SBUS_B;
	SBUS_DATA[0] = 0x0F;
	SBUS_B = (CH_Rec[0] - 500);
	SBUS_DATA[1] = SBUS_B & 0xFF;
	SBUS_B = (SBUS_B | (((uint32_t)(CH_Rec[1] - 500)) << 11)) >> 8;
	SBUS_DATA[2] = SBUS_B & 0xFF;
	SBUS_B = (SBUS_B | (((uint32_t)(CH_Rec[2] - 500)) << 14)) >> 8;
	SBUS_DATA[3] = SBUS_B & 0xFF;
	SBUS_B = SBUS_B >> 8;
	SBUS_DATA[4] = SBUS_B & 0xFF;
	SBUS_B = (SBUS_B | (((uint32_t)(CH_Rec[3] - 500)) << 9)) >> 8;
	SBUS_DATA[5] = SBUS_B & 0xFF;
	SBUS_B = (SBUS_B | (((uint32_t)(CH_Rec[4] - 500)) << 12)) >> 8;
	SBUS_DATA[6] = SBUS_B & 0xFF;
	SBUS_B = (SBUS_B | (((uint32_t)(CH_Rec[5] - 500)) << 15)) >> 8;
	SBUS_DATA[7] = SBUS_B & 0xFF;
	SBUS_B = SBUS_B >> 8;
	SBUS_DATA[8] = SBUS_B & 0xFF;
	SBUS_B = (SBUS_B | (((uint32_t)(CH_Rec[6] - 500)) << 10)) >> 8;
	SBUS_DATA[9] = SBUS_B & 0xFF;
	SBUS_B = (SBUS_B | (((uint32_t)(CH_Rec[7] - 500)) << 13)) >> 8;
	SBUS_DATA[10] = SBUS_B & 0xFF;
	SBUS_B = SBUS_B >> 8;
	SBUS_DATA[11] = SBUS_B & 0xFF;
	SBUS_B = SBUS_B >> 8;
	SBUS_B = (CH_Rec[8] - 500);
	SBUS_DATA[12] = SBUS_B & 0xFF;
	SBUS_B = (SBUS_B | (((uint32_t)(CH_Rec[9] - 500)) << 11)) >> 8;
	SBUS_DATA[13] = SBUS_B & 0xFF;
	SBUS_B = (SBUS_B | (((uint32_t)(CH_Rec[10] - 500)) << 14)) >> 8;
	SBUS_DATA[14] = SBUS_B & 0xFF;
	SBUS_B = SBUS_B >> 8;
	SBUS_DATA[15] = SBUS_B & 0xFF;
	SBUS_B = (SBUS_B | (((uint32_t)(CH_Rec[11] - 500)) << 9)) >> 8;
	SBUS_DATA[16] = SBUS_B & 0xFF;
	SBUS_B = (SBUS_B | (((uint32_t)(CH_Rec[12] - 500)) << 12)) >> 8;
	SBUS_DATA[17] = SBUS_B & 0xFF;
	SBUS_B = (SBUS_B | (((uint32_t)(CH_Rec[13] - 500)) << 15)) >> 8;
	SBUS_DATA[18] = SBUS_B & 0xFF;
	SBUS_B = SBUS_B >> 8;
	SBUS_DATA[19] = SBUS_B & 0xFF;
	SBUS_B = (SBUS_B | (((uint32_t)(CH_Rec[14] - 500)) << 10)) >> 8;
	SBUS_DATA[20] = SBUS_B & 0xFF;
	SBUS_B = (SBUS_B | (((uint32_t)(CH_Rec[15] - 500)) << 13)) >> 8;
	SBUS_DATA[21] = SBUS_B & 0xFF;
	SBUS_B = SBUS_B >> 8;
	SBUS_DATA[22] = SBUS_B & 0xFF;
	SBUS_B = SBUS_B >> 8;
	
	SBUS_DATA[23] = 0x00;// | 0x0C;			//无信号（RSSI低）时应加入失控标记
	SBUS_DATA[24] = 0x00;
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

