#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h" 



#define KEY_RIGHT 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_1) //
#define KEY_LEFT 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)	//PE3 
#define KEY_UP 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2) //PE2
#define KEY_DOWN 	GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_0)	//PA0




void KEY_Init(void);	//IO初始化
u8 KEY_Scan(u8);  		//按键扫描函数	

#endif
