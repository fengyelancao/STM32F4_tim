#include "exti.h"
#include "sys.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//外部中断 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/4
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

#include "usart.h"

//外部中断初始化程序
//初始化PE2~4,PA0为中断输入.
void EXTIX_Init(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure; 
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//使能GPIOA,GPIOE时钟
	/* 选择按键1的引脚 */ 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_2; //KEY0 KEY1 KEY2对应引脚
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//上拉
  GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化GPIOE2,3,4
	/* 使用上面的结构体初始化按键 */
	GPIO_Init(GPIOA, &GPIO_InitStructure); 

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//使能SYSCFG时钟
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource0);//PA0 连接到中断线0
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource2);//PA0 连接到中断线0
	/* 配置EXTI_Line0 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;//LINE0
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿触发 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//使能LINE0
	EXTI_Init(&EXTI_InitStructure);//配置
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;//外部中断0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);//配置
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line2;//LINE0
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿触发 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//使能LINE0
	EXTI_Init(&EXTI_InitStructure);//配置
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;//外部中断0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);//配置

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
//外部中断0服务程序
//void EXTI0_IRQHandler(void)
//{
//	if(PWM_cap_IO_0 == 1)	 
//	{
//		TIM2_count_0 = TIM_GetCounter(TIM2);
//		end_exit_0();

//	}		 
//	 EXTI_ClearITPendingBit(EXTI_Line0); //清除LINE0上的中断标志位 
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
//	 EXTI_ClearITPendingBit(EXTI_Line1); //清除LINE0上的中断标志位 
//}












