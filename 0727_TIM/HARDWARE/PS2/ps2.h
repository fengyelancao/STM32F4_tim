#ifndef __PS2_H
#define __PS2_H	 
#include "delay.h"	   
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//PS2 ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/13
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////			   
//����ӿڶ���
//PS2���� 		  				    
#define PS2_SCL PDin(1)	//PC11
#define PS2_SDA PDin(3)	//PC10
//PS2���
#define PS2_SCL_OUT PDout(1)	//PC11
#define PS2_SDA_OUT PDout(3)	//PC10

//����PS2_SCL�������״̬.		  
/*#define PS2_SET_SCL_IN()  {GPIOD->MODER&=0XFFFFFFF3;GPIOD->PUPDR&=0XFFFFFFF7;GPIOD->PUPDR|=0X00000004;}
#define PS2_SET_SCL_OUT() {GPIOD->MODER&=0XFFFFFFF7;GPIOD->MODER|=0X00000004;}	  
//����PS2_SDA�������״̬.		  
#define PS2_SET_SDA_IN()  {GPIOD->MODER&=0XFFFFFF3F;GPIOD->PUPDR&=0XFFFFFF7F;GPIOD->PUPDR|=0X00000040;}
#define PS2_SET_SDA_OUT() {GPIOD->MODER&=0XFFFFFF7F;GPIOD->MODER|=0X00000040;} 
*/
#define MOUSE    0X20 //���ģʽ
#define KEYBOARD 0X10 //����ģʽ
#define CMDMODE  0X00 //��������
//PS2_Status��ǰ״̬��־
//[5:4]:��ǰ������ģʽ;[7]:���յ�һ������
//[6]:У�����;[3:0]:�յ������ݳ���;	 
extern u8 PS2_Status;       //����Ϊ����ģʽ
extern u8 PS2_DATA_BUF[16]; //ps2���ݻ�����
//extern u8 MOUSE_ID;

void PS2_Init(void);
u8 PS2_Send_Cmd(u8 cmd);
void PS2_Set_Int(u8 en);
u8 PS2_Get_Byte(void);
void PS2_En_Data_Report(void);  
void PS2_Dis_Data_Report(void);		
void PS2_SET_SDA_OUT(void);
void PS2_SET_SCL_OUT(void);
void PS2_SET_SDA_IN(void);
void PS2_SET_SCL_IN(void);  				    
#endif





















