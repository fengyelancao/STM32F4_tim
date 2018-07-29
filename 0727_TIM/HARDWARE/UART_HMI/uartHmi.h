#ifndef _UARTHMI_H
#define _UARTHMI_H
#include "sys.h" 
#include "stdio.h"

/*------------使用其他串口------------*/
#define HMI_USARTX USART3                 //串口几
#define RCC_HMI    RCC_APB1Periph_USART3  //串口时钟//注意时钟的总线分为两种，可能不一样

#define HMI_USART_GPIOX GPIOB  				//串口IO口

#define HMI_USART_AF GPIO_AF_USART3
#define HMI_USART_AF_TXD_Pin GPIO_PinSource10
#define HMI_USART_AF_RXD_Pin GPIO_PinSource11

#define HMI_USART_TXD_Pin GPIO_Pin_10
#define HMI_USART_RXD_Pin GPIO_Pin_11

#define HMI_USART_IRQ USART3_IRQn
#define HMI_USART_IRQHandler USART3_IRQHandler
	
/*---------------------全局变量----------------------*/
//串口屏改变参数
extern char s_HMI[20];  

/*----------------------全局函数----------------------*/
//串口频初始化
void uart_HMI_init(u32 bound);
//串口屏改变参数的性质
void Usart_HMI_SendString(char *str);

/*--------------------内部函数--------------------*/
 void Usart_HMI_SendByte( USART_TypeDef * pUSARTx, uint8_t ch);
#endif

