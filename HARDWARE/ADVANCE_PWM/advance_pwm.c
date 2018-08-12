#include "advance_pwm.h"
#include "exti.h"
#include "usart.h"

/*
 * 注意：TIM_TimeBaseInitTypeDef结构体里面有5个成员，TIM6和TIM7的寄存器里面只有
 * TIM_Prescaler和TIM_Period，所以使用TIM6和TIM7的时候只需初始化这两个成员即可，
 * 另外三个成员是通用定时器和高级定时器才有.
 *-----------------------------------------------------------------------------
 * TIM_Prescaler         都有
 * TIM_CounterMode			 TIMx,x[6,7]没有，其他都有（基本定时器）
 * TIM_Period            都有
 * TIM_ClockDivision     TIMx,x[6,7]没有，其他都有(基本定时器)
 * TIM_RepetitionCounter TIMx,x[1,8]才有(高级定时器)
 *-----------------------------------------------------------------------------
 */
static void TIM_Mode_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;
	/*开启定时器相关的GPIO外设时钟*/
	RCC_AHB1PeriphClockCmd (ADVANCE_OCPWM_GPIO_CLK, ENABLE); 
	// 开启TIMx_CLK,x[1,8] 
	RCC_APB2PeriphClockCmd(ADVANCE_TIM_CLK, ENABLE); 

	/* 指定引脚复用功能 */
	GPIO_PinAFConfig(ADVANCE_OCPWM_GPIO_PORT,ADVANCE_OCPWM_PINSOURCE, ADVANCE_OCPWM_AF); 

	/* 定时器功能引脚初始化 */															   
	GPIO_InitStructure.GPIO_Pin = ADVANCE_OCPWM_PIN;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 
	GPIO_Init(ADVANCE_OCPWM_GPIO_PORT, &GPIO_InitStructure);

	/* 累计 TIM_Period个后产生一个更新或者中断*/		
	//当定时器从0计数到1023，即为1024次，为一个定时周期
	TIM_TimeBaseStructure.TIM_Period = 199-1;
	// 高级控制定时器时钟源TIMxCLK = HCLK=168MHz 
	// 设定定时器频率为=TIMxCLK/(TIM_Prescaler+1)=100000Hz
	TIM_TimeBaseStructure.TIM_Prescaler = 21-1;	
	// 采样时钟分频
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	// 计数方式
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	// 重复计数器
	TIM_TimeBaseStructure.TIM_RepetitionCounter=16;	
	// 初始化定时器TIMx, x[1,8]
	TIM_TimeBaseInit(ADVANCE_TIM, &TIM_TimeBaseStructure);

	/*PWM模式配置*/
	//配置为PWM模式1
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	TIM_OCInitStructure.TIM_Pulse = 100;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	//使能通道1
	TIM_OC1Init(ADVANCE_TIM, &TIM_OCInitStructure);	 

	/* 使能通道1重载 */
	TIM_OC1PreloadConfig(ADVANCE_TIM, TIM_OCPreload_Enable);	
	// 清除定时器更新中断标志位
	TIM_ClearFlag(ADVANCE_TIM, TIM_FLAG_Update);

	// 开启定时器更新中断
	TIM_ITConfig(ADVANCE_TIM,TIM_IT_Update,ENABLE);
	// 使能定时器
	TIM_Cmd(ADVANCE_TIM, ENABLE);	
	/* 主动输出使能 */
	TIM_CtrlPWMOutputs(ADVANCE_TIM, ENABLE);
	NVIC_InitStructure.NVIC_IRQChannel = TIM8_UP_TIM13_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、
}

/**
  * @brief  初始化高级控制定时器
  * @param  无
  * @retval 无
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


//通用定时器3中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器3!
void TIM5_CH1_Cap_Init(u32 arr,u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_ICInitTypeDef  TIM5_ICInitStructure;

	RCC_APB1PeriphClockCmd(TIM_Cap_CLK,ENABLE);  	//TIM5时钟使能    
	RCC_AHB1PeriphClockCmd(TIM_Cap_GPIO_CLK, ENABLE); 	//使能PORTA时钟	

	GPIO_InitStructure.GPIO_Pin = TIM_Cap_GPIO_Pin; //GPIOA0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN; //下拉
	GPIO_Init(TIM_Cap_GPIO,&GPIO_InitStructure); //初始化PA0

	GPIO_PinAFConfig(TIM_Cap_GPIO,TIM_Cap_GPIO_source_pin,TIM_Cap_GPIO_AF); //PA0复用位定时器5
	GPIO_PinAFConfig(TIM_Cap_GPIO,GPIO_PinSource1,TIM_Cap_GPIO_AF); 

	TIM_TimeBaseStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseStructure.TIM_Period=arr;   //自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV4; 

	TIM_TimeBaseInit(TIM_Cap,&TIM_TimeBaseStructure);


	//初始化TIM5输入捕获参数
	TIM5_ICInitStructure.TIM_Channel = TIM_Cap_Channel; //CC1S=01 	选择输入端 IC1映射到TI1上
	TIM5_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
	TIM5_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
	TIM5_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频 
	TIM5_ICInitStructure.TIM_ICFilter = 0xb;//IC1F=0000 配置输入滤波器 不滤波
	TIM_ICInit(TIM_Cap, &TIM5_ICInitStructure);

	TIM5_ICInitStructure.TIM_Channel = TIM_Channel_2; //CC1S=01 	选择输入端 IC1映射到TI1上
	TIM5_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
	TIM5_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
	TIM5_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频 
	TIM5_ICInitStructure.TIM_ICFilter = 0xb;//IC1F=0000 配置输入滤波器 不滤波
	TIM_ICInit(TIM_Cap, &TIM5_ICInitStructure);

	TIM_ITConfig(TIM_Cap,TIM_IT_Update|TIM_Cap_IT|TIM_IT_CC2,ENABLE);//允许更新中断 ,允许CC1IE捕获中断	

	TIM_Cmd(TIM_Cap,ENABLE ); 	//使能定时器5


	NVIC_InitStructure.NVIC_IRQChannel = TIM_Cap_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、
}

void TIM5_cap_star(void)
{
	time_cap_flag = 0;
//	TIM_SetCounter(TIM5,0);
}

u32 tim_cap_1 = 0;
u32 tim_cap_2 = 0;
u8 time_cap_flag = 0;
//定时器5中断服务程序	 
void TIM_Cap_IRQHandler(void)
{ 		
	if(time_cap_flag == 0)
	{
		if(TIM_GetITStatus(TIM_Cap, TIM_IT_Update) != RESET)//溢出
		{
			
		}
		else if(TIM_GetITStatus(TIM_Cap, TIM_Cap_IT) != RESET)//捕获1发生捕获事件
		{	
			tim_cap_1 = TIM_GetCapture1(TIM5);
			
//			printf("\r\n%d\r\n",tim_cap_1);
			
		}		
		else if(TIM_GetITStatus(TIM_Cap, TIM_IT_CC2) != RESET)//捕获1发生捕获事件
		{	
			tim_cap_2 = TIM_GetCapture2(TIM5);
			time_cap_flag = 1;
		}
	}			
	TIM_ClearITPendingBit(TIM_Cap,TIM_IT_CC2 | TIM_Cap_IT|TIM_IT_Update); //清除中断标志位
}


/*********************************************END OF FILE**********************/
