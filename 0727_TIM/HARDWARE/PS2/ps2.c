#include "ps2.h" 
#include "usart.h"
#include "keyboard.h"

//PS2产生的时钟频率在10~20Khz(最大33K)
//高/低电平的持续时间为25~50us之间.	  
			 
//PS2_Status当前状态标志
//[7]:接收到一次数据;[6]:校验错误;[5:4]:当前工作的模式;[3:0]:收到的数据长度;
u8 PS2_Status=CMDMODE; //默认为命令模式 
u8 PS2_DATA_BUF[16];   //ps2数据缓存区

//位计数器
u8 BIT_Count=0;
	   
//中断15~10处理函数
//每11个bit,为接收1个字节
//每接收完一个包(11位)后,设备至少会等待50ms再发送下一个包
//只做了鼠标部分,键盘部分暂时未加入
//CHECK OK 2010/5/2

GPIO_InitTypeDef   GPIO_InitStructure;

EXTI_InitTypeDef EXTI_InitStructure;

void EXTI1_IRQHandler (void)
{	  	 
	static u8 tempdata=0;
	static u8 parity=0;  	   
	if(EXTI_GetITStatus(EXTI_Line1)==SET)	//中断11产生了相应的中断
	{  
		EXTI_ClearITPendingBit(EXTI_Line1); //清除LINE11上的中断标志位

		if(BIT_Count==0)
		{
			parity=0;
			tempdata=0;
		}
		BIT_Count++;    
		if(BIT_Count>1&&BIT_Count<10)//这里获得数据
		{	  
			tempdata>>=1;
			if(PS2_SDA)
			{
				tempdata|=0x80;
				parity++;//记录1的个数
			}   
		}
		else if(BIT_Count==10)//得到校验位
		{
			if(PS2_SDA)parity|=0x80;//校验位为1
		}
		
		if(BIT_Count==11)//接收到1个字节的数据了
		{
 			BIT_Count=parity&0x7f;//取得1的个数	  
			if(((BIT_Count%2==0)&&(parity&0x80))||((BIT_Count%2==1)&&(parity&0x80)==0))//奇偶校验OK
			{					    
				//PS2_Status|=1<<7;//标记得到数据	   
				BIT_Count=PS2_Status&0x0f;		
				PS2_DATA_BUF[BIT_Count]=tempdata;//保存数据
				if(BIT_Count<15)PS2_Status++;    //数据长度加1
				BIT_Count=PS2_Status&0x30;	     //得到模式	  
				switch(BIT_Count)
				{
					case CMDMODE://命令模式下,每收到一个字节都会产生接收完成
						PS2_Dis_Data_Report();//禁止数据传输
						PS2_Status|=1<<7; //标记得到数据
						break;
					case KEYBOARD://键盘模式下，会接收到三个字节：例如‘A’,MCU会接收到十六进制数1C,F0,1C。
						//if(KEYBOARD_ID1==0XAB&&KEYBOARD_ID2==0X83) //程序尚没完整，没将所有键盘控制符加入。
						//{
							if((PS2_Status&0x0f)==3)
							{
								if(KEYBOARD_COUNT<600)KEYBOARD_COUNT++;	  //屏幕总共能显示600个16*8 ASCII码字符
								else KEYBOARD_COUNT=0;
								PS2_Status|=1<<7;//标记得到数据
								PS2_Dis_Data_Report();//禁止数据传输
							}
						//}
						break;

				}		   		 
			}
			else
			{
				PS2_Status|=1<<6;//标记校验错误
				PS2_Status&=0xf0;//清除接收数据计数器
				//PS2_Status&=0X7F;//标记得到数据
			}
			BIT_Count=0;
		} 	 	  
	}			  
}
//禁止数据传输
//把时钟线拉低,禁止数据传输
//CHECK OK 2010/5/2
void PS2_Dis_Data_Report(void)
{
	PS2_Set_Int(0);   //关闭中断
	PS2_SET_SCL_OUT();//设置SCL为输出
	PS2_SCL_OUT=0;    //抑制传输
}
//使能数据传输
//释放时钟线
//CHECK OK 2010/5/2
void PS2_En_Data_Report(void)
{
	PS2_SET_SCL_IN(); //设置SCL为输入
	PS2_SET_SDA_IN(); //SDA IN
	PS2_SCL_OUT=1;    //上拉   
	PS2_SDA_OUT=1; 
	PS2_Set_Int(1);   //开启中断
}

//PS2中断屏蔽设置
//en:1，开启;0，关闭;
//CHECK OK 2010/5/2
void PS2_Set_Int(u8 en)
{
	EXTI_ClearITPendingBit(EXTI_Line1);  //清除EXTI11线路挂起位
 
	if(en)
	{
	 	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	}
	else
	{
	    EXTI_InitStructure.EXTI_LineCmd = DISABLE;
	}
	 EXTI_Init(&EXTI_InitStructure);		//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

}
//等待PS2时钟线sta状态改变
//sta:1，等待变为1;0，等待变为0;
//返回值:0，时钟线变成了sta;1，超时溢出;
//CHECK OK 2010/5/2
u8 Wait_PS2_Scl(u8 sta)
{
	u16 t=0;
	sta=!sta;
	while(PS2_SCL==sta)
	{
		delay_us(1);
		t++;
		if(t>16000)return 1;//时间溢出 (设备会在10ms内检测这个状态)
	}
	return 0;//被拉低了
}
//在发送命令/数据之后,等待设备应带,该函数用来获取应答
//返回得到的值 
//返回0，且PS2_Status.6=1，则产生了错误
//CHECK OK 2010/5/2
u8 PS2_Get_Byte(void)
{
	u16 t=0;
	u8 temp=0;
	while(1)//最大等待55ms
	{
		t++;
		delay_us(10);
		if(PS2_Status&0x80)//得到了一次数据
		{
			temp=PS2_DATA_BUF[PS2_Status&0x0f-1];
			PS2_Status&=0x70;//清除计数器，接收到数据标记
			break;	
		}else if(t>5500||PS2_Status&0x40)break;//超时溢出/接收错误	   
	}
	PS2_En_Data_Report();//使能数据传输
	return temp;    
}	    
//发送一个命令到PS2.
//返回值:0，无错误,其他,错误代码
u8 PS2_Send_Cmd(u8 cmd)
{
	u8 i;
	u8 high=0;//记录1的个数		 
	PS2_Set_Int(0);   //屏蔽中断
	PS2_SET_SCL_OUT();//设置SCL为输出
	PS2_SET_SDA_OUT();//SDA OUT
	PS2_SCL_OUT=0;//拉低时钟线
	delay_us(120);//保持至少100us
	PS2_SDA_OUT=0;//拉低数据线
	delay_us(10);
	PS2_SET_SCL_IN();//释放时钟线,这里PS2设备得到第一个位,开始位
	PS2_SCL_OUT=1;
	if(Wait_PS2_Scl(0)==0)//等待时钟拉低
	{									  
		for(i=0;i<8;i++)
		{
			if(cmd&0x01)
			{
			    PS2_SDA_OUT=1;
				high++;
			}else PS2_SDA_OUT=0;   
			cmd>>=1;
			//这些地方没有检测错误,因为这些地方不会产生死循环
			Wait_PS2_Scl(1);//等待时钟拉高 发送8个位
			Wait_PS2_Scl(0);//等待时钟拉低
		}
		if((high%2)==0)PS2_SDA_OUT=1;//发送校验位 10
		else PS2_SDA_OUT=0;
		Wait_PS2_Scl(1); //等待时钟拉高 10位
		Wait_PS2_Scl(0); //等待时钟拉低
		PS2_SDA_OUT=1;   //发送停止位  11	  
 		Wait_PS2_Scl(1);//等待时钟拉高 11位
		PS2_SET_SDA_IN();//SDA in
		Wait_PS2_Scl(0);//等待时钟拉低
		if(PS2_SDA==0)Wait_PS2_Scl(1);//等待时钟拉高 12位 
		else 
		{
			PS2_En_Data_Report();
			return 1;//发送失败
		}		
	}
	else 
	{
		PS2_En_Data_Report();
		return 2;//发送失败
	}
	PS2_En_Data_Report();
	return 0;    //发送成功
}
//PS2初始化				   
//CHECK OK 2010/5/2
void PS2_Init(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_3; //KEY0 KEY1 KEY2对应引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化GPIOE2,3,4
	PS2_SDA_OUT = 1;
	PS2_SCL_OUT = 1;
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource1);//PE2 连接到中断线2
	

	//中断线初始化	   	 
	EXTI_InitStructure.EXTI_Line = EXTI_Line1;//LINE0
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿触发 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//使能LINE0
	EXTI_Init(&EXTI_InitStructure);//配置

	//中断分组初始化
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;//外部中断0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);//配置
	 
} 

void PS2_SET_SCL_IN(void)
{
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; //KEY0 KEY1 KEY2对应引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化GPIOE2,3,4
}

void PS2_SET_SDA_IN(void)
{
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; //KEY0 KEY1 KEY2对应引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化GPIOE2,3,4
}

void PS2_SET_SCL_OUT(void)
{
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; //KEY0 KEY1 KEY2对应引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输入模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//上拉
	GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化GPIOE2,3,4
}

void PS2_SET_SDA_OUT(void)
{
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; //KEY0 KEY1 KEY2对应引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输入模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//上拉
	GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化GPIOE2,3,4
}













