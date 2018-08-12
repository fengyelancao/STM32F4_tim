#include "advance_pwm.h"
#include "exti.h"
#include "usart.h"

/*
 * ע�⣺TIM_TimeBaseInitTypeDef�ṹ��������5����Ա��TIM6��TIM7�ļĴ�������ֻ��
 * TIM_Prescaler��TIM_Period������ʹ��TIM6��TIM7��ʱ��ֻ���ʼ����������Ա���ɣ�
 * ����������Ա��ͨ�ö�ʱ���͸߼���ʱ������.
 *-----------------------------------------------------------------------------
 * TIM_Prescaler         ����
 * TIM_CounterMode			 TIMx,x[6,7]û�У��������У�������ʱ����
 * TIM_Period            ����
 * TIM_ClockDivision     TIMx,x[6,7]û�У���������(������ʱ��)
 * TIM_RepetitionCounter TIMx,x[1,8]����(�߼���ʱ��)
 *-----------------------------------------------------------------------------
 */
static void TIM_Mode_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef GPIO_InitStructure;
	/*������ʱ����ص�GPIO����ʱ��*/
	RCC_AHB1PeriphClockCmd (ADVANCE_OCPWM_GPIO_CLK, ENABLE); 
	// ����TIMx_CLK,x[1,8] 
	RCC_APB2PeriphClockCmd(ADVANCE_TIM_CLK, ENABLE); 

	/* ָ�����Ÿ��ù��� */
	GPIO_PinAFConfig(ADVANCE_OCPWM_GPIO_PORT,ADVANCE_OCPWM_PINSOURCE, ADVANCE_OCPWM_AF); 

	/* ��ʱ���������ų�ʼ�� */															   
	GPIO_InitStructure.GPIO_Pin = ADVANCE_OCPWM_PIN;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 
	GPIO_Init(ADVANCE_OCPWM_GPIO_PORT, &GPIO_InitStructure);

	/* �ۼ� TIM_Period�������һ�����»����ж�*/		
	//����ʱ����0������1023����Ϊ1024�Σ�Ϊһ����ʱ����
	TIM_TimeBaseStructure.TIM_Period = 199-1;
	// �߼����ƶ�ʱ��ʱ��ԴTIMxCLK = HCLK=168MHz 
	// �趨��ʱ��Ƶ��Ϊ=TIMxCLK/(TIM_Prescaler+1)=100000Hz
	TIM_TimeBaseStructure.TIM_Prescaler = 21-1;	
	// ����ʱ�ӷ�Ƶ
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	// ������ʽ
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	// �ظ�������
	TIM_TimeBaseStructure.TIM_RepetitionCounter=16;	
	// ��ʼ����ʱ��TIMx, x[1,8]
	TIM_TimeBaseInit(ADVANCE_TIM, &TIM_TimeBaseStructure);

	/*PWMģʽ����*/
	//����ΪPWMģʽ1
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	TIM_OCInitStructure.TIM_Pulse = 100;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	//ʹ��ͨ��1
	TIM_OC1Init(ADVANCE_TIM, &TIM_OCInitStructure);	 

	/* ʹ��ͨ��1���� */
	TIM_OC1PreloadConfig(ADVANCE_TIM, TIM_OCPreload_Enable);	
	// �����ʱ�������жϱ�־λ
	TIM_ClearFlag(ADVANCE_TIM, TIM_FLAG_Update);

	// ������ʱ�������ж�
	TIM_ITConfig(ADVANCE_TIM,TIM_IT_Update,ENABLE);
	// ʹ�ܶ�ʱ��
	TIM_Cmd(ADVANCE_TIM, ENABLE);	
	/* �������ʹ�� */
	TIM_CtrlPWMOutputs(ADVANCE_TIM, ENABLE);
	NVIC_InitStructure.NVIC_IRQChannel = TIM8_UP_TIM13_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����
}

/**
  * @brief  ��ʼ���߼����ƶ�ʱ��
  * @param  ��
  * @retval ��
  */
void TIMx_Configuration(void)
{  
  TIM_Mode_Config();
}


__IO u8 send_PWM_flag = 0;
void TIM8_UP_TIM13_IRQHandler(void)
{
	if ( TIM_GetITStatus( ADVANCE_TIM, TIM_IT_Update) != RESET ) 
	{	
		send_PWM_flag = 1;
		TIM_SetCounter(TIM5,0);
		TIM_Cmd(ADVANCE_TIM, DISABLE);
		TIM_ClearITPendingBit(ADVANCE_TIM , TIM_IT_Update); 
	}	
}


void TIM8_send_pwm_star(void)
{
	TIM_Cmd(ADVANCE_TIM, ENABLE);
}

void change_pwm_fre(u8 PWM_fre)
{
	switch(PWM_fre)
	{
		case 20 :
		{
			TIM_SetAutoreload(ADVANCE_TIM , 4200);
			TIM_SetCompare1(ADVANCE_TIM , 2100);
		}break;
		case 30 :
		{
			TIM_SetAutoreload(ADVANCE_TIM , 2800);
			TIM_SetCompare1(ADVANCE_TIM , 1400);
		}break;
		case 40 :
		{
			TIM_SetAutoreload(ADVANCE_TIM , 2100);
			TIM_SetCompare1(ADVANCE_TIM , 1025);
		}break;
		case 50 :
		{
			TIM_SetAutoreload(ADVANCE_TIM , 1680);
			TIM_SetCompare1(ADVANCE_TIM , 840);
		}break;
	}
}


//ͨ�ö�ʱ��3�жϳ�ʼ��
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz
//����ʹ�õ��Ƕ�ʱ��3!
void TIM5_CH1_Cap_Init(u32 arr,u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_ICInitTypeDef  TIM5_ICInitStructure;

	RCC_APB1PeriphClockCmd(TIM_Cap_CLK,ENABLE);  	//TIM5ʱ��ʹ��    
	RCC_AHB1PeriphClockCmd(TIM_Cap_GPIO_CLK, ENABLE); 	//ʹ��PORTAʱ��	

	GPIO_InitStructure.GPIO_Pin = TIM_Cap_GPIO_Pin; //GPIOA0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN; //����
	GPIO_Init(TIM_Cap_GPIO,&GPIO_InitStructure); //��ʼ��PA0

	GPIO_PinAFConfig(TIM_Cap_GPIO,TIM_Cap_GPIO_source_pin,TIM_Cap_GPIO_AF); //PA0����λ��ʱ��5
	GPIO_PinAFConfig(TIM_Cap_GPIO,GPIO_PinSource1,TIM_Cap_GPIO_AF); 

	TIM_TimeBaseStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_Period=arr;   //�Զ���װ��ֵ
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV4; 

	TIM_TimeBaseInit(TIM_Cap,&TIM_TimeBaseStructure);


	//��ʼ��TIM5���벶�����
	TIM5_ICInitStructure.TIM_Channel = TIM_Cap_Channel; //CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
	TIM5_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
	TIM5_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
	TIM5_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //���������Ƶ,����Ƶ 
	TIM5_ICInitStructure.TIM_ICFilter = 0xb;//IC1F=0000 ���������˲��� ���˲�
	TIM_ICInit(TIM_Cap, &TIM5_ICInitStructure);

	TIM5_ICInitStructure.TIM_Channel = TIM_Channel_2; //CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
	TIM5_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
	TIM5_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
	TIM5_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //���������Ƶ,����Ƶ 
	TIM5_ICInitStructure.TIM_ICFilter = 0xb;//IC1F=0000 ���������˲��� ���˲�
	TIM_ICInit(TIM_Cap, &TIM5_ICInitStructure);

	TIM_ITConfig(TIM_Cap,TIM_IT_Update|TIM_Cap_IT|TIM_IT_CC2,ENABLE);//��������ж� ,����CC1IE�����ж�	

	TIM_Cmd(TIM_Cap,ENABLE ); 	//ʹ�ܶ�ʱ��5


	NVIC_InitStructure.NVIC_IRQChannel = TIM_Cap_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����
}

void TIM5_cap_star(void)
{
	time_cap_flag = 0;
//	TIM_SetCounter(TIM5,0);
}

u32 tim_cap_1 = 0;
u32 tim_cap_2 = 0;
u8 time_cap_flag = 0;
//��ʱ��5�жϷ������	 
void TIM_Cap_IRQHandler(void)
{ 		
	if(time_cap_flag == 0)
	{
		if(TIM_GetITStatus(TIM_Cap, TIM_IT_Update) != RESET)//���
		{
			
		}
		else if(TIM_GetITStatus(TIM_Cap, TIM_Cap_IT) != RESET)//����1���������¼�
		{	
			tim_cap_1 = TIM_GetCapture1(TIM5);
			
//			printf("\r\n%d\r\n",tim_cap_1);
			
		}		
		else if(TIM_GetITStatus(TIM_Cap, TIM_IT_CC2) != RESET)//����1���������¼�
		{	
			tim_cap_2 = TIM_GetCapture2(TIM5);
			time_cap_flag = 1;
		}
	}			
	TIM_ClearITPendingBit(TIM_Cap,TIM_IT_CC2 | TIM_Cap_IT|TIM_IT_Update); //����жϱ�־λ
}


/*********************************************END OF FILE**********************/
