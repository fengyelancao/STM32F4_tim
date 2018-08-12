#include "uartHmi.h"
	
char s_HMI[20]={0};






//初始化IO 串口3 
//bound:波特率
void uart_HMI_init(u32 bound)
	{
   //GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); //使能GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_HMI,ENABLE);//不同的串口有不同的总线
 
	//串口3对应引脚复用映射
	GPIO_PinAFConfig(HMI_USART_GPIOX,HMI_USART_AF_TXD_Pin,HMI_USART_AF); 
	GPIO_PinAFConfig(HMI_USART_GPIOX,HMI_USART_AF_RXD_Pin,HMI_USART_AF); 
	
	//USART1端口配置
    GPIO_InitStructure.GPIO_Pin = HMI_USART_RXD_Pin | HMI_USART_TXD_Pin; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;   //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(HMI_USART_GPIOX,&GPIO_InitStructure); //GPIO初始化

/*----------------------------USART3 初始化设置-------------------------*/
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(HMI_USARTX, &USART_InitStructure); //初始化串口1
	
	USART_ITConfig(HMI_USARTX, USART_IT_RXNE, ENABLE);//开启相关中断
	USART_Cmd(HMI_USARTX, ENABLE);  //使能串口1 
/*---------------------------------Usart1 NVIC 配置----------------------------*/
	NVIC_InitStructure.NVIC_IRQChannel = HMI_USART_IRQ;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
		
	
	
}
/*
	中断函数主要用于触摸屏的发送回来的值，
	在UART_HMI中设置发送回来的是什么值
*/
char addt_flag = 0;
void HMI_USART_IRQHandler(void)                	//串口1中断服务程序
{
	u8 Res;
	if(USART_GetITStatus(HMI_USARTX, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res =USART_ReceiveData(HMI_USARTX);
		switch(Res)
		{
			case 0xff :break;
			case 0xFE :addt_flag = 1; break; 
			case 0xFD :addt_flag = 0; break;
		}

		USART_ClearITPendingBit(HMI_USARTX, USART_IT_RXNE);
	} 
	
	if (USART_GetFlagStatus(HMI_USARTX, USART_FLAG_ORE) != RESET)//注意！不能使用if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)来判断  
    {  

        USART_ReceiveData(HMI_USARTX);  

    } 
	
} 
/*****************  发送一个字符 **********************/
void Usart_HMI_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
{
  /* 发送一个字节数据到 USART */
  USART_SendData(pUSARTx,ch);
  /* 等待发送数据寄存器为空 */
  while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
}

 /*****************  发送字符串 **********************/
 void Usart_HMI_SendString(char *str)
 {
   unsigned int k=0;
   do {
   Usart_HMI_SendByte( HMI_USARTX, *(str + k) );
   k++;
   } while (*(str + k)!='\0');
   //串口屏使用数据结束位
   Usart_HMI_SendByte( HMI_USARTX,0xFF);
   Usart_HMI_SendByte( HMI_USARTX,0xFF);
   Usart_HMI_SendByte( HMI_USARTX,0xFF);
   /* 等待发送完成 */
   while (USART_GetFlagStatus(HMI_USARTX,USART_FLAG_TC)==RESET) ;
 }

