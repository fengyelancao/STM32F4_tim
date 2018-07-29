#ifndef __KEYBORD_H
#define __KEYBORD_H

#include "ps2.h"
#include "usart.h"

#define OTHER_KEY_PRESS 0

//HOST->DEVICE的命令集
#define KEYBORDRESET    					0XFF //引起键盘进入"Reset"模式
#define RESEND								0XFE //用于只是在接收中出现的错误键盘的响应就是重发送最后的扫描码或者命令回应给主机但是0xFE绝不会作为Resend命令的回应而被发送 
#define Set_Key_Type_Make					0XFD //允许主机指定一个按键只发送通码这个按键不发送断码或进行机打重复指定的按键采用它的第三套扫描码 
#define Set_Key_Type_Make_Break				0XFC //只有通码和断码是使能的(机打被禁止了)
#define Set_Key_Type_Typematic 				0XFB //通码和机打是使能的,而断码被禁止
#define Set_All_Keys_Typematic_Make_Break	0XFA //缺省设置,所有键的通码断码和机打重复都使能,除了Print Screen键,它在第一套和第二套中没有断码
#define Set_All_Keys_Make			   		0XF9 //所有键都只发送通码,断码和机打重复被禁止
#define Set_All_Keys_Make_Break				0XF8 //所有键都只发送通码和断码,机打重复被禁止
#define Set_All_Keys_Typematic				0XF7 //仅断码被禁止,通码和机打重复是使能的
#define Set_Default							0XF6 //载入缺省的机打速率/延时(10.9cps/500ms),按键类型(所有按键都使能机打/通码/断码)以及第二套扫描码集
#define Disable								0XF5 //键盘停止扫描,载入缺省值,（键Set Default命令），等待进一步指令
#define Enable								0XF4 //在用上一条命令禁止键盘后,重新使能键盘
#define Set_Typematic_Rate_Delay			0XF3 //主机在这条命令后会发送一个字节的参数来定义机打速率和延时,具体含义如下:  
												 //						Repeat Rate 
												 //Bits0-4 Rate Bits0-4 Rate Bits0-4 Rate Bits0-4 Rate 												
												 //00h 2.0 08h 4.0 10h 8.0 18h 16.0 
												 //01h 2.1 09h 4.3 11h 8.6 19h 17.1 
												 //02h 2.3 0Ah 4.6 12h 9.2 1Ah 18.5 
												 //03h 2.5 0Bh 5.0 13h 10.0 1Bh 20.0 
												 //04h 2.7 0Ch 5.5 14h 10.9 1Ch 21.8 
												 //05h 3.0 0Dh 6.0 15h 12.0 1Dh 24.0 
												 //06h 3.3 0Eh 6.7 16h 13.3 1Eh 26.7 
												 //07h 3.7 0Fh 7.5 17h 15.0 1Fh 30.0 
												 //					   Delay
												 //Bits 5-6  Delay (seconds) 
												 //00		 0.25
												 //01		 0.50
												 //10		 0.75
												 //11		 1.00
#define Read_ID								0XF2 //键盘回应两个字节的设备ID,0xAB,0X83
#define Set_Scan_Code_Set					0XF0 //主机在这个命令后发送一个字节的参数,是定键盘使用哪套扫描码集,参数字节可以是0x01,0x02或0x03分别选择扫描码集第一套第二套或第三套,如果要获得当前正在使用的扫描码集只要发送带0x00参数的本命令即可
#define Echo								0XEE //键盘用Echo(0xEE)回应
#define	Set_Reset_LEDs						0XED //主机在本命令后跟随一个参数字节用于指示键盘上Num Lock, Caps Lock, and Scroll Lock LED的状态,这个参数字节的定义如下:
												 //MSB       																LSB  
												 //Always 0   Always 0 Always 0 Always 0 Always 0 Caps_Lock Num_Lock Scroll_Lock
												 //"Scroll Lock" - Scroll Lock LED off(0)/on(1) 
												 // "Num Lock" - Num Lock LED off(0)/on(1) 
												 //"Caps Lock" - Caps Lock LED off(0)/on(1) 

extern u8 KEYBOARD_ID1;
extern u8 KEYBOARD_ID2;
extern u16 KEYBOARD_COUNT;

void Decode(u8 scancode);
u8 Init_Keyboard(void);								 
												 
#endif
