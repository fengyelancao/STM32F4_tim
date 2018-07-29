#include "exti.h"
#include "sys.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//�ⲿ�ж� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/4
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

#include "usart.h"

//�ⲿ�жϳ�ʼ������
//��ʼ��PE2~4,PA0Ϊ�ж�����.
void EXTIX_Init(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure; 
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//ʹ��GPIOA,GPIOEʱ��
	/* ѡ�񰴼�1������ */ 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_2; //KEY0 KEY1 KEY2��Ӧ����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//��ͨ����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//����
  GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��GPIOE2,3,4
	/* ʹ������Ľṹ���ʼ������ */
	GPIO_Init(GPIOA, &GPIO_InitStructure); 

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//ʹ��SYSCFGʱ��
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource0);//PA0 ���ӵ��ж���0
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource2);//PA0 ���ӵ��ж���0
	/* ����EXTI_Line0 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;//LINE0
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//�ж��¼�
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //�����ش��� 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//ʹ��LINE0
	EXTI_Init(&EXTI_InitStructure);//����
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;//�ⲿ�ж�0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);//����
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line2;//LINE0
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//�ж��¼�
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //�����ش��� 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//ʹ��LINE0
	EXTI_Init(&EXTI_InitStructure);//����
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;//�ⲿ�ж�0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);//����

}


void end_exit_0(void)
{
	EXTI->IMR &= ~(EXTI_Line0);
}
void end_exit_2(void)
{
	EXTI->IMR &= ~(EXTI_Line2);
}


void star_exit_0(void)
{
	EXTI->IMR |= EXTI_Line0;
}
void star_exit_2(void)
{
	EXTI->IMR |= EXTI_Line2;
}
u32 TIM2_count_0 = 0;
u32 TIM2_count_2 = 0;
//u8 EXIT_0_flag = 0;
//u8 EXIT_2_flag = 0;
//�ⲿ�ж�0�������
//void EXTI0_IRQHandler(void)
//{
//	if(PWM_cap_IO_0 == 1)	 
//	{
//		TIM2_count_0 = TIM_GetCounter(TIM2);
//		end_exit_0();

//	}		 
//	 EXTI_ClearITPendingBit(EXTI_Line0); //���LINE0�ϵ��жϱ�־λ 
//}	

//void EXTI2_IRQHandler(void)
//{
//	if(PWM_cap_IO_1 == 1)	 
//	{

//		TIM2_count_2 = TIM_GetCounter(TIM2);
//		end_exit_2();
//		TIM2_end();
//		printf("tim0 :%d  ",TIM2_count_0);
//		printf("tim2 :%d  ",TIM2_count_2);
//		printf("tim2  - time0 :%d\r\n",TIM2_count_2 - TIM2_count_0);
//		
//	}		 
//	 EXTI_ClearITPendingBit(EXTI_Line1); //���LINE0�ϵ��жϱ�־λ 
//}












