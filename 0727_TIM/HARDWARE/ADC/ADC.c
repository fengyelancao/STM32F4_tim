/**
  ******************************************************************************
  * @file    bsp_bsp_adc.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   adc驱动
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 F407 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
#include "adc.h"
#include "delay.h"
__IO uint32_t ADC_ConvertedValue[1024];

static void Rheostat_ADC_GPIO_Config(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
	
	// 使能 GPIO 时钟
	RCC_AHB1PeriphClockCmd(RHEOSTAT_ADC_GPIO_CLK, ENABLE);
		
	// 配置 IO
	GPIO_InitStructure.GPIO_Pin = RHEOSTAT_ADC_GPIO_PIN|GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;	    
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ; //不上拉不下拉
	GPIO_Init(RHEOSTAT_ADC_GPIO_PORT, &GPIO_InitStructure);		
}


static void TIM_PWMOUTPUT_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	// 开启TIMx_CLK,x[2,3,4,5,12,13,14] 
  RCC_APB1PeriphClockCmd(GENERAL_TIM_CLK, ENABLE); 

  /* 累计 TIM_Period个后产生一个更新或者中断*/		
  //当定时器从0计数到8399，即为8400次，为一个定时周期
  TIM_TimeBaseStructure.TIM_Period = 1262-1;       
	
	// 通用控制定时器时钟源TIMxCLK = HCLK/2=84MHz 
	// 设定定时器频率为=TIMxCLK/(TIM_Prescaler+1)=100KHz
  TIM_TimeBaseStructure.TIM_Prescaler = 130-1;	
  // 采样时钟分频
  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
  // 计数方式
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	
	// 初始化定时器TIMx, x[2,3,4,5,12,13,14] 
	TIM_TimeBaseInit(GENERAL_TIM, &TIM_TimeBaseStructure);
	
	/*PWM模式配置*/
	/* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    //配置为PWM模式1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
  TIM_OCInitStructure.TIM_Pulse = 631-1;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  	  //当定时器计数值小于CCR1_Val时为高电平
  TIM_OC3Init(GENERAL_TIM, &TIM_OCInitStructure);	 //使能通道1
  
	/*使能通道1重载*/
	TIM_OC3PreloadConfig(GENERAL_TIM, TIM_OCPreload_Enable);
	
	// 使能定时器
	TIM_Cmd(GENERAL_TIM, DISABLE);	
}
static void Rheostat_ADC_Mode_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	// ------------------DMA Init 结构体参数 初始化--------------------------
	// ADC1使用DMA2，数据流0，通道0，这个是手册固定死的
	// 开启DMA时钟
	RCC_AHB1PeriphClockCmd(RHEOSTAT_ADC_DMA_CLK, ENABLE); 
	// 外设基址为：ADC 数据寄存器地址
	DMA_InitStructure.DMA_PeripheralBaseAddr = RHEOSTAT_ADC_CDR_ADDR;	
	// 存储器地址，实际上就是一个内部SRAM的变量	
	DMA_InitStructure.DMA_Memory0BaseAddr = (u32)&ADC_ConvertedValue;  
	// 数据传输方向为外设到存储器	
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;	
	// 缓冲区大小为，指一次传输的数据量
	DMA_InitStructure.DMA_BufferSize = 1024;	
	// 外设寄存器只有一个，地址不用递增
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	// 存储器地址固定
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 
	// // 外设数据大小为半字，即两个字节 
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word; 
	//	存储器数据大小也为半字，跟外设数据大小相同
	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Word;	
	// 循环传输模式
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	// DMA 传输通道优先级为高，当使用一个DMA通道时，优先级设置不影响
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	// 禁止DMA FIFO	，使用直连模式
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;  
	// FIFO 大小，FIFO模式禁止时，这个不用配置	
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;  
	// 选择 DMA 通道，通道存在于流中
	DMA_InitStructure.DMA_Channel = RHEOSTAT_ADC_DMA_CHANNEL; 
	//初始化DMA流，流相当于一个大的管道，管道里面有很多通道
	DMA_Init(RHEOSTAT_ADC_DMA_STREAM, &DMA_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

	DMA_ITConfig(RHEOSTAT_ADC_DMA_STREAM,DMA_IT_TC,ENABLE);
	// 使能DMA流
	DMA_Cmd(RHEOSTAT_ADC_DMA_STREAM, ENABLE);
	
	// 开启ADC时钟
	RCC_APB2PeriphClockCmd(RHEOSTAT_ADC1_CLK , ENABLE);
  RCC_APB2PeriphClockCmd(RHEOSTAT_ADC2_CLK , ENABLE);
	
  // -------------------ADC Init 结构体 参数 初始化--------------------------
	ADC_StructInit(&ADC_InitStructure);
	// 独立ADC模式
  ADC_CommonInitStructure.ADC_Mode = ADC_DualMode_RegSimult;
  // 时钟为fpclk x分频	
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;
  // 禁止DMA直接访问模式	
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_2;
  // 采样时间间隔	
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_20Cycles;  
  ADC_CommonInit(&ADC_CommonInitStructure);
	
  // -------------------ADC Init 结构体 参数 初始化--------------------------
  // ADC 分辨率
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  // 禁止扫描模式，多通道采集才需要	
  ADC_InitStructure.ADC_ScanConvMode = DISABLE; 
  // 连续转换	
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; 
  //禁止外部边沿触发
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Falling;
  //外部触发通道，本例子使用软件触发，此值随便赋值即可
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_CC3;
  //数据右对齐	
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  //转换通道 1个
  ADC_InitStructure.ADC_NbrOfConversion = 1; 
	
  ADC_Init(RHEOSTAT_ADC1, &ADC_InitStructure);
  //---------------------------------------------------------------------------
	
   // 配置 ADC 通道转换顺序为1，第一个转换，采样时间为3个时钟周期
  ADC_RegularChannelConfig(RHEOSTAT_ADC1, ADC_Channel_12, 1, ADC_SampleTime_3Cycles);   
  //---------------------------------------------------------------------------
	
	ADC_Init(RHEOSTAT_ADC2, &ADC_InitStructure);
  // 配置 ADC 通道转换顺序为1，第一个转换，采样时间为3个时钟周期
  ADC_RegularChannelConfig(RHEOSTAT_ADC2, ADC_Channel_11, 1, ADC_SampleTime_3Cycles);   
  //---------------------------------------------------------------------------

  // 使能DMA请求 after last transfer (multi-ADC mode)
  ADC_MultiModeDMARequestAfterLastTransferCmd(ENABLE);
  // 使能ADC DMA
  ADC_DMACmd(RHEOSTAT_ADC1, ENABLE);
	
  // 使能ADC
  ADC_Cmd(RHEOSTAT_ADC1, ENABLE);  
  ADC_Cmd(RHEOSTAT_ADC2, ENABLE);  
  //开始adc转换，软件触发
  ADC_SoftwareStartConv(RHEOSTAT_ADC1);

}
u8 ADC_finish_flag = 0;
void DMA2_Stream0_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA2_Stream0,DMA_IT_TCIF0) != RESET)
    {
		ADC_finish_flag = 1;
        DMA_ClearITPendingBit(DMA2_Stream0,DMA_IT_TCIF0);
		TIM_Cmd(TIM2, DISABLE);	
//		printf("ADC_ok\r\n");
    }
}


void Rheostat_Init(void)
{
	Rheostat_ADC_GPIO_Config();
	Rheostat_ADC_Mode_Config();
	TIM_PWMOUTPUT_Config();
}





void Set_PWM_fre(u16 pre,u16 AUTO_load,u16 pulse)
{
	TIM_PrescalerConfig(GENERAL_TIM,pre-1,TIM_PSCReloadMode_Immediate);
	TIM_SetAutoreload(GENERAL_TIM ,AUTO_load-1);
	TIM_SetCompare3(GENERAL_TIM , pulse-1);
	
}



