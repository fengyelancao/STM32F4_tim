#include "uartHmi.h"
	
char s_HMI[20]={0};






//��ʼ��IO ����3 
//bound:������
void uart_HMI_init(u32 bound)
	{
   //GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_HMI,ENABLE);//��ͬ�Ĵ����в�ͬ������
 
	//����3��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(HMI_USART_GPIOX,HMI_USART_AF_TXD_Pin,HMI_USART_AF); 
	GPIO_PinAFConfig(HMI_USART_GPIOX,HMI_USART_AF_RXD_Pin,HMI_USART_AF); 
	
	//USART1�˿�����
    GPIO_InitStructure.GPIO_Pin = HMI_USART_RXD_Pin | HMI_USART_TXD_Pin; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;   //���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(HMI_USART_GPIOX,&GPIO_InitStructure); //GPIO��ʼ��

/*----------------------------USART3 ��ʼ������-------------------------*/
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(HMI_USARTX, &USART_InitStructure); //��ʼ������1
	
	USART_ITConfig(HMI_USARTX, USART_IT_RXNE, ENABLE);//��������ж�
	USART_Cmd(HMI_USARTX, ENABLE);  //ʹ�ܴ���1 
/*---------------------------------Usart1 NVIC ����----------------------------*/
	NVIC_InitStructure.NVIC_IRQChannel = HMI_USART_IRQ;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
		
	
	
}
/*
	�жϺ�����Ҫ���ڴ������ķ��ͻ�����ֵ��
	��UART_HMI�����÷��ͻ�������ʲôֵ
*/
char addt_flag = 0;
void HMI_USART_IRQHandler(void)                	//����1�жϷ������
{
	u8 Res;
	if(USART_GetITStatus(HMI_USARTX, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
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
	
	if (USART_GetFlagStatus(HMI_USARTX, USART_FLAG_ORE) != RESET)//ע�⣡����ʹ��if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)���ж�  
    {  

        USART_ReceiveData(HMI_USARTX);  

    } 
	
} 
/*****************  ����һ���ַ� **********************/
void Usart_HMI_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
{
  /* ����һ���ֽ����ݵ� USART */
  USART_SendData(pUSARTx,ch);
  /* �ȴ��������ݼĴ���Ϊ�� */
  while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
}

 /*****************  �����ַ��� **********************/
 void Usart_HMI_SendString(char *str)
 {
   unsigned int k=0;
   do {
   Usart_HMI_SendByte( HMI_USARTX, *(str + k) );
   k++;
   } while (*(str + k)!='\0');
   //������ʹ�����ݽ���λ
   Usart_HMI_SendByte( HMI_USARTX,0xFF);
   Usart_HMI_SendByte( HMI_USARTX,0xFF);
   Usart_HMI_SendByte( HMI_USARTX,0xFF);
   /* �ȴ�������� */
   while (USART_GetFlagStatus(HMI_USARTX,USART_FLAG_TC)==RESET) ;
 }

