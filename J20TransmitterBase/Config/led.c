#include "led.h"
	    
//LED ������IO��ʼ��
void LED_Init(void)
{
 
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ��PC�˿�ʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;				 //LED0-->PB.5 �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOC13
	
	//��ʼ�� ����������PA10	  �������
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10;	
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA10
	GPIO_ResetBits(GPIOA,GPIO_Pin_10);						 //PA10 �����
	GPIO_SetBits(GPIOC,GPIO_Pin_13);						 //PC13 �����

}
 