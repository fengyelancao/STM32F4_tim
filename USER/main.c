#include "stm32f4xx.h"
#include "usart.h"
#include "delay.h"
#include "ps2.h" 
#include "uartHmi.h"
#include "keyboard.h"
#include "key.h"
#include "advance_pwm.h"
#include "exti.h"
#include <string.h>
#include "adc.h"
#define MEDIO_V 1.677
#define V1 1.167
#define V2 1.322

float WAVE_data[100] = {0,1,2,3,4,5,6,7,8,9,10,11,1,20,30,50};


void main_init(void);
void WAVE_fliter(void);
int main(void)
{	
   float time_measure = 0;
	static u8 WF_times = 0;
	main_init();
//	EXTIX_Init();
//	KEY_Init();
	TIMx_Configuration();
	TIM5_CH1_Cap_Init(0xFFFFFFFF - 1,1-1);
	Rheostat_Init();
//	printf("ok");
  while (1)
  {      
	  if(send_PWM_flag == 1)
	  {
		  delay_ms(10);
		  TIM_Cmd(TIM2, ENABLE);
		while(ADC_finish_flag == 0);
		  if(time_cap_flag == 1)
		  {
//			  printf("CC1 :%ld",tim_cap_1);
//			  printf("CC2 :%ld",tim_cap_2);
//			  printf("CC1 - CC2 :%ld\r\n",tim_cap_2 - tim_cap_1);
			  time_measure = tim_cap_1 - (tim_cap_2 - tim_cap_1)*(MEDIO_V - V2)/(V2 - V1);
			  WAVE_data[WF_times++] = time_measure;
			  if(WF_times >= 100)
			  {
				  WAVE_fliter();
				  WF_times = 0;
			  }			  
			  time_cap_flag = 0;  
		  }
		  TIM8_send_pwm_star();
  }
//	KEY_Scan(0);
  }
}

void main_init(void)
{
	u8 err;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init(168);
	uart_init(115200);
    uart_HMI_init(115200);
    PS2_Init();						     
	delay_ms(800);            //等待上电复位完成	
	
RST:err=Init_Keyboard();
  if(err==0)
    {
        sprintf(s_HMI,"a4.txt=\"%s\"","PS2键盘初始化成功");
        Usart_HMI_SendString(s_HMI);
		printf("OK");
    }
  else
  {
	  printf("ERR:%d",err);
        sprintf(s_HMI,"a4.txt=\"PS2键盘初始化失败：err%d\"",err);
        Usart_HMI_SendString(s_HMI);
    goto RST;
  } 
}



void WAVE_fliter(void)
{
	u8 WF_times_i = 0;
	u8 WF_times_j = 0;
	float WF_temp = 0;
	double WF_sum = 0;
	for(WF_times_j = 0; WF_times_j <100 ; WF_times_j++)
	{
		for(WF_times_i = 0; WF_times_i <100;WF_times_i++)
		{
			if(WAVE_data[WF_times_j] > WAVE_data[WF_times_i])
			{
				WF_temp = WAVE_data[WF_times_i];
				WAVE_data[WF_times_i] = WAVE_data[WF_times_j] ;
				WAVE_data[WF_times_j]  = WF_temp;
			}
		}
	}
	
	for(WF_times_j = 35 ; WF_times_j < 65 ; WF_times_j++)
	{
		WF_sum += WAVE_data[WF_times_j];
	}	
//		for(WF_times_j = 0 ; WF_times_j < 100 ; WF_times_j++)
//	{
//		printf("          %lf\r\n",WAVE_data[WF_times_j]);
//	}	
	WF_sum = WF_sum/40.0;
	printf("%lf\r\n",WF_sum);
	
}

