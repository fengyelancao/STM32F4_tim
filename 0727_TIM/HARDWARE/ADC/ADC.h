#ifndef __ADC_H
#define	__ADC_H

#include "stm32f4xx.h"

extern __IO uint32_t ADC_ConvertedValue[1024];

// ADC GPIO 宏定义
#define RHEOSTAT_ADC_GPIO_PORT    GPIOC
#define RHEOSTAT_ADC_GPIO_PIN     GPIO_Pin_2
#define RHEOSTAT_ADC_GPIO_CLK     RCC_AHB1Periph_GPIOC

// ADC 序号宏定义
#define RHEOSTAT_ADC1             ADC1
#define RHEOSTAT_ADC1_CLK         RCC_APB2Periph_ADC1
#define RHEOSTAT_ADC2             ADC2
#define RHEOSTAT_ADC2_CLK         RCC_APB2Periph_ADC2
#define RHEOSTAT_ADC_CHANNEL      ADC_Channel_12
// ADC CDR寄存器宏定义，ADC转换后的数字值则存放在这里
#define RHEOSTAT_ADC_CDR_ADDR    ((uint32_t)0x40012308)

// ADC DMA 通道宏定义，这里我们使用DMA传输
#define RHEOSTAT_ADC_DMA_CLK      RCC_AHB1Periph_DMA2
#define RHEOSTAT_ADC_DMA_CHANNEL  DMA_Channel_0
#define RHEOSTAT_ADC_DMA_STREAM   DMA2_Stream0


#define GENERAL_OCPWM_PIN             GPIO_Pin_2             
#define GENERAL_OCPWM_GPIO_PORT       GPIOA                      
#define GENERAL_OCPWM_GPIO_CLK        RCC_AHB1Periph_GPIOA
#define GENERAL_OCPWM_PINSOURCE		  GPIO_PinSource2
#define GENERAL_OCPWM_AF			  GPIO_AF_TIM2

#define GENERAL_TIM           		    TIM2
#define GENERAL_TIM_CLK       		    RCC_APB1Periph_TIM2

#define GENERAL_TIM_IRQn					    TIM2_IRQn
#define GENERAL_TIM_IRQHandler        TIM2_IRQHandler


extern u8 ADC_finish_flag;
void Rheostat_Init(void);
void ADC_FFT(void);
void base_function(void);
void ADC_WAVE_paly(void);
#endif /* __BSP_ADC_H */



