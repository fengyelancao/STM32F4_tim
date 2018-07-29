#ifndef __ADVANCE_PWM_H
#define	__ADVANCE_PWM_H

#include "stm32f4xx.h"

/* 定时器 */
#define ADVANCE_TIM           		    TIM8
#define ADVANCE_TIM_CLK       		    RCC_APB2Periph_TIM8

/* TIM8通道1输出引脚 */
#define ADVANCE_OCPWM_PIN             GPIO_Pin_6              
#define ADVANCE_OCPWM_GPIO_PORT       GPIOC                      
#define ADVANCE_OCPWM_GPIO_CLK        RCC_AHB1Periph_GPIOC
#define ADVANCE_OCPWM_PINSOURCE				GPIO_PinSource6
#define ADVANCE_OCPWM_AF							GPIO_AF_TIM8


/*输入捕获GPIO*/
#define TIM_Cap_GPIO_CLK         RCC_AHB1Periph_GPIOA
#define TIM_Cap_GPIO_Pin         GPIO_Pin_0|GPIO_Pin_1
#define TIM_Cap_GPIO             GPIOA
#define TIM_Cap_GPIO_AF          GPIO_AF_TIM5
#define TIM_Cap_GPIO_source_pin  GPIO_PinSource0
/*输入捕获定时器*/
#define TIM_Cap_Channel        TIM_Channel_1
#define TIM_Cap_IT             TIM_IT_CC1
#define TIM_Cap                TIM5
#define TIM_Cap_CLK            RCC_APB1Periph_TIM5
/*输入捕获中断*/
#define TIM_Cap_IRQn           TIM5_IRQn
/*-------------函数---------------*/
#define TIM_GetCapture         TIM_GetCapture1
#define TIM_Cap_IRQHandler     TIM5_IRQHandler
extern __IO u8 send_PWM_flag ;
extern u8 time_cap_flag ;
extern u32 tim_cap_1;
extern u32 tim_cap_2;

void TIMx_Configuration(void);
void change_pwm_fre(u8 PWM_fre);
void TIM8_send_pwm_star(void);
void TIM5_CH1_Cap_Init(u32 arr,u16 psc);


void TIM2_star(void);
void TIM2_end(void);
#endif /* __ADVANCE_TIM_H */

