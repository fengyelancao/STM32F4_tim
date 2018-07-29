#include "keyboard.h"
#include "uartHmi.h"
#include "usart.h"

u8 KEYBOARD_ID1,KEYBOARD_ID2;
u16 KEYBOARD_COUNT;


void PS2_SCAN(void)
{
	u8 err;
	 if(PS2_Status&0x80)//到了一次数据
    {   
//      TIM_Cmd(TIM3, DISABLE); //关闭定时器，保护按键的输入、与信号传输
      Decode(PS2_DATA_BUF[0]);
      PS2_Status=KEYBOARD;
      PS2_En_Data_Report();//使能数据报告
      if(PS2_DATA_BUF[0]==0xe0)
      {
        err=Init_Keyboard();
		  sprintf(s_HMI,"a1.txt=\"PS2键盘初始化失败：err%d\"",err);
      }
      PS2_DATA_BUF[0] = 0;
//      TIM_Cmd(TIM3, ENABLE);
    }
}

//处理KEYBOARD的数据，键盘控制符到ASCII码的映射关系	 
void Decode(u8 scancode)  
{     
	static u32 sum_data=0;
	static u8 data[20]={0};
	static s8 data_temp=0;
	static s8 sum_temp=0;
	char num=0;
	static char enter_flag=1;	

		switch(scancode)//开始翻译扫描码  
		{  			
			case 0x70 : num = '0'; break;  
			case 0x69 : num = '1'; break; 
			case 0x72 : num = '2'; break; 
			case 0x7a : num = '3'; break; 
			case 0x6b : num = '4'; break; 
			case 0x73 : num = '5'; break; 
			case 0x74 : num = '6'; break;
			case 0x6c : num = '7'; break;
			case 0x75 : num = '8'; break;      
			case 0x7d : num = '9'; break;
			
			case 0x71 : num = OTHER_KEY_PRESS;break;// '.'
			case 0x7c :num = OTHER_KEY_PRESS;break;
			case 0x7b :;break;

			case 0x66: // backspace键按下时处理  
				{
					data[--data_temp]=0;					
					num=OTHER_KEY_PRESS;
					sprintf(s_HMI,"A1.txt=\"%s\"",data);
					Usart_HMI_SendString(s_HMI);
				}break;  
			case 0x79 :num = OTHER_KEY_PRESS;break;// '+';

			case 0xe0: 		
			// 回车键按下时处理
				{
					enter_flag=1;					//已经摁下了ENTER键用来标识第一个数字键摁下
					num = OTHER_KEY_PRESS;			//非数字键摁下

					for(sum_temp=0;sum_temp<data_temp;sum_temp++) //求数组data求和
					{
						sum_data=sum_data*10+(data[sum_temp]-0x30);
					}					
					printf("sunm_data:%d\r\n",sum_data);
					sprintf(s_HMI,"A1.txt=\"%d\"",sum_data);
					Usart_HMI_SendString(s_HMI);
					sum_data=0;
				}break;
				
			default: break; 				
		}


		if(num!=OTHER_KEY_PRESS)//摁下数字键
		{
			if(enter_flag==1)
			{
				enter_flag=0;
				data_temp =0;
			}			
			data[data_temp++] = num;
			data[data_temp]=0;
			printf("%s\r\n",data);
			sprintf(s_HMI,"A1.txt=\"%s\"",data);
			Usart_HMI_SendString(s_HMI);
		}		  
}	 

//初始化键盘
//返回:0,初始化成功
//其他:错误代码
//CHECK OK 2011/11/20/10:37
u8 Init_Keyboard(void)
{
	u8 t;		 	 	 
	PS2_Status=CMDMODE;       //进入命令模式
	t=PS2_Send_Cmd(KEYBORDRESET); //复位键盘	
	delay_us(50);	
	if(t!=0)return 1;
	t=PS2_Get_Byte();			  
    if(t!=0XFA)return 2;
	t=0;
	while((PS2_Status&0x80)==0)//等待复位完毕 
	{
		t++;
		delay_ms(10);      
		if(t>50)return 3;
	}
	PS2_Get_Byte();//得到0XAA		 
	//进入键盘模式的特殊初始化序列
	
	PS2_Send_Cmd(Set_Reset_LEDs);       	//引起键盘进入"Reset"模式
    if(PS2_Get_Byte()!=0XFA)return 4;   	//传输失败
	PS2_Send_Cmd(0X00);						//关闭指示键盘上Num Lock, Caps Lock, and Scroll Lock LED
    if(PS2_Get_Byte()!=0XFA)return 5;   	//传输失败
	PS2_Send_Cmd(Read_ID);					//键盘回应两个字节的设备ID,0xAB,0X83
    if(PS2_Get_Byte()!=0XFA)return 6;   	//传输失败
	KEYBOARD_ID1=PS2_Get_Byte();			//设备KEYBOARD_ID1,0xAB
	KEYBOARD_ID2=PS2_Get_Byte();			//设备KEYBOARD_ID2,0X83
	PS2_Send_Cmd(Set_Reset_LEDs);       	//引起键盘进入"Reset"模式
    if(PS2_Get_Byte()!=0XFA)return 7;   	//传输失败
	PS2_Send_Cmd(0X02);						//点亮指示键盘上Num Lock灯
    if(PS2_Get_Byte()!=0XFA)return 8;   	//传输失败
	PS2_Send_Cmd(Set_Typematic_Rate_Delay); //定义机打速率和延时
    if(PS2_Get_Byte()!=0XFA)return 9;		//传输失败
	PS2_Send_Cmd(0X20);						//机打速率和延时：30.0 reports/sec 、500 ms 
    if(PS2_Get_Byte()!=0XFA)return 10;		//传输失败
	PS2_Send_Cmd(Enable);					//使能键盘
    if(PS2_Get_Byte()!=0XFA)return 11;		//传输失败
	PS2_Send_Cmd(Set_Typematic_Rate_Delay); //定义机打速率和延时
    if(PS2_Get_Byte()!=0XFA)return 12;		//传输失败
	PS2_Send_Cmd(0X17);						//机打速率和延时：15.0 reports/sec 、250 ms
    if(PS2_Get_Byte()!=0XFA)return 13;		//传输失败

	PS2_Status=KEYBOARD;//进入键盘模式
	return 0;//无错误,初始化成功   
}

