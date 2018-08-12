#include "keyboard.h"
#include "uartHmi.h"
#include "usart.h"

u8 KEYBOARD_ID1,KEYBOARD_ID2;
u16 KEYBOARD_COUNT;


void PS2_SCAN(void)
{
	u8 err;
	 if(PS2_Status&0x80)//����һ������
    {   
//      TIM_Cmd(TIM3, DISABLE); //�رն�ʱ�����������������롢���źŴ���
      Decode(PS2_DATA_BUF[0]);
      PS2_Status=KEYBOARD;
      PS2_En_Data_Report();//ʹ�����ݱ���
      if(PS2_DATA_BUF[0]==0xe0)
      {
        err=Init_Keyboard();
		  sprintf(s_HMI,"a1.txt=\"PS2���̳�ʼ��ʧ�ܣ�err%d\"",err);
      }
      PS2_DATA_BUF[0] = 0;
//      TIM_Cmd(TIM3, ENABLE);
    }
}

//����KEYBOARD�����ݣ����̿��Ʒ���ASCII���ӳ���ϵ	 
void Decode(u8 scancode)  
{     
	static u32 sum_data=0;
	static u8 data[20]={0};
	static s8 data_temp=0;
	static s8 sum_temp=0;
	char num=0;
	static char enter_flag=1;	

		switch(scancode)//��ʼ����ɨ����  
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

			case 0x66: // backspace������ʱ����  
				{
					data[--data_temp]=0;					
					num=OTHER_KEY_PRESS;
					sprintf(s_HMI,"A1.txt=\"%s\"",data);
					Usart_HMI_SendString(s_HMI);
				}break;  
			case 0x79 :num = OTHER_KEY_PRESS;break;// '+';

			case 0xe0: 		
			// �س�������ʱ����
				{
					enter_flag=1;					//�Ѿ�������ENTER��������ʶ��һ�����ּ�����
					num = OTHER_KEY_PRESS;			//�����ּ�����

					for(sum_temp=0;sum_temp<data_temp;sum_temp++) //������data���
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


		if(num!=OTHER_KEY_PRESS)//�������ּ�
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

//��ʼ������
//����:0,��ʼ���ɹ�
//����:�������
//CHECK OK 2011/11/20/10:37
u8 Init_Keyboard(void)
{
	u8 t;		 	 	 
	PS2_Status=CMDMODE;       //��������ģʽ
	t=PS2_Send_Cmd(KEYBORDRESET); //��λ����	
	delay_us(50);	
	if(t!=0)return 1;
	t=PS2_Get_Byte();			  
    if(t!=0XFA)return 2;
	t=0;
	while((PS2_Status&0x80)==0)//�ȴ���λ��� 
	{
		t++;
		delay_ms(10);      
		if(t>50)return 3;
	}
	PS2_Get_Byte();//�õ�0XAA		 
	//�������ģʽ�������ʼ������
	
	PS2_Send_Cmd(Set_Reset_LEDs);       	//������̽���"Reset"ģʽ
    if(PS2_Get_Byte()!=0XFA)return 4;   	//����ʧ��
	PS2_Send_Cmd(0X00);						//�ر�ָʾ������Num Lock, Caps Lock, and Scroll Lock LED
    if(PS2_Get_Byte()!=0XFA)return 5;   	//����ʧ��
	PS2_Send_Cmd(Read_ID);					//���̻�Ӧ�����ֽڵ��豸ID,0xAB,0X83
    if(PS2_Get_Byte()!=0XFA)return 6;   	//����ʧ��
	KEYBOARD_ID1=PS2_Get_Byte();			//�豸KEYBOARD_ID1,0xAB
	KEYBOARD_ID2=PS2_Get_Byte();			//�豸KEYBOARD_ID2,0X83
	PS2_Send_Cmd(Set_Reset_LEDs);       	//������̽���"Reset"ģʽ
    if(PS2_Get_Byte()!=0XFA)return 7;   	//����ʧ��
	PS2_Send_Cmd(0X02);						//����ָʾ������Num Lock��
    if(PS2_Get_Byte()!=0XFA)return 8;   	//����ʧ��
	PS2_Send_Cmd(Set_Typematic_Rate_Delay); //����������ʺ���ʱ
    if(PS2_Get_Byte()!=0XFA)return 9;		//����ʧ��
	PS2_Send_Cmd(0X20);						//�������ʺ���ʱ��30.0 reports/sec ��500 ms 
    if(PS2_Get_Byte()!=0XFA)return 10;		//����ʧ��
	PS2_Send_Cmd(Enable);					//ʹ�ܼ���
    if(PS2_Get_Byte()!=0XFA)return 11;		//����ʧ��
	PS2_Send_Cmd(Set_Typematic_Rate_Delay); //����������ʺ���ʱ
    if(PS2_Get_Byte()!=0XFA)return 12;		//����ʧ��
	PS2_Send_Cmd(0X17);						//�������ʺ���ʱ��15.0 reports/sec ��250 ms
    if(PS2_Get_Byte()!=0XFA)return 13;		//����ʧ��

	PS2_Status=KEYBOARD;//�������ģʽ
	return 0;//�޴���,��ʼ���ɹ�   
}

