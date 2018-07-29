#ifndef __KEYBORD_H
#define __KEYBORD_H

#include "ps2.h"
#include "usart.h"

#define OTHER_KEY_PRESS 0

//HOST->DEVICE�����
#define KEYBORDRESET    					0XFF //������̽���"Reset"ģʽ
#define RESEND								0XFE //����ֻ���ڽ����г��ֵĴ�����̵���Ӧ�����ط�������ɨ������������Ӧ����������0xFE��������ΪResend����Ļ�Ӧ�������� 
#define Set_Key_Type_Make					0XFD //��������ָ��һ������ֻ����ͨ��������������Ͷ������л����ظ�ָ���İ����������ĵ�����ɨ���� 
#define Set_Key_Type_Make_Break				0XFC //ֻ��ͨ��Ͷ�����ʹ�ܵ�(���򱻽�ֹ��)
#define Set_Key_Type_Typematic 				0XFB //ͨ��ͻ�����ʹ�ܵ�,�����뱻��ֹ
#define Set_All_Keys_Typematic_Make_Break	0XFA //ȱʡ����,���м���ͨ�����ͻ����ظ���ʹ��,����Print Screen��,���ڵ�һ�׺͵ڶ�����û�ж���
#define Set_All_Keys_Make			   		0XF9 //���м���ֻ����ͨ��,����ͻ����ظ�����ֹ
#define Set_All_Keys_Make_Break				0XF8 //���м���ֻ����ͨ��Ͷ���,�����ظ�����ֹ
#define Set_All_Keys_Typematic				0XF7 //�����뱻��ֹ,ͨ��ͻ����ظ���ʹ�ܵ�
#define Set_Default							0XF6 //����ȱʡ�Ļ�������/��ʱ(10.9cps/500ms),��������(���а�����ʹ�ܻ���/ͨ��/����)�Լ��ڶ���ɨ���뼯
#define Disable								0XF5 //����ֹͣɨ��,����ȱʡֵ,����Set Default������ȴ���һ��ָ��
#define Enable								0XF4 //������һ�������ֹ���̺�,����ʹ�ܼ���
#define Set_Typematic_Rate_Delay			0XF3 //���������������ᷢ��һ���ֽڵĲ���������������ʺ���ʱ,���庬������:  
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
#define Read_ID								0XF2 //���̻�Ӧ�����ֽڵ��豸ID,0xAB,0X83
#define Set_Scan_Code_Set					0XF0 //����������������һ���ֽڵĲ���,�Ƕ�����ʹ������ɨ���뼯,�����ֽڿ�����0x01,0x02��0x03�ֱ�ѡ��ɨ���뼯��һ�׵ڶ��׻������,���Ҫ��õ�ǰ����ʹ�õ�ɨ���뼯ֻҪ���ʹ�0x00�����ı������
#define Echo								0XEE //������Echo(0xEE)��Ӧ
#define	Set_Reset_LEDs						0XED //�����ڱ���������һ�������ֽ�����ָʾ������Num Lock, Caps Lock, and Scroll Lock LED��״̬,��������ֽڵĶ�������:
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
