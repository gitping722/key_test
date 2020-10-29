#include "stm32f10x.h"
#include "user.h"


unsigned char flag=0;
unsigned char KeyValue=0;
uchar buff[3];



/************************************************
函数名称：void RCC_Config(void)
函数功能：对用到的时钟进行初始化，打开各模块时钟
入口参数：无
返回值：  无
说明  ：  主要是打开GPIOF和TIM3的时钟
************************************************/
void RCC_Config(void)
{
	
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOA
	                        | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD
	                        | RCC_APB2Periph_GPIOB,ENABLE);	//GPIOF时钟打开
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);	//TIM3时钟打开
	
}

/************************************************
函数名称：void GPIO_Config(void)
函数功能：对LED灯端口进行初始化
入口参数：无
返回值：  无
说明  ：  用到了PF6管脚，设置为推挽输出
************************************************/
void GPIO_Config(void)
{
	
  GPIO_InitTypeDef GPIO_InitStructure;
	//key
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0| GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	  //上拉输入
  GPIO_Init(GPIOA,&GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	  //上拉输入
  GPIO_Init(GPIOC,&GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	  //上拉输入
  GPIO_Init(GPIOD,&GPIO_InitStructure);
	
	//bezzer
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	  
//  GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	//led
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7| GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	  
  GPIO_Init(GPIOF,&GPIO_InitStructure);
	GPIOF->ODR |= 0x3c0;

}

/************************************************
函数名称：void TIM_Config(void)
函数功能：对TIM3进行初始化
入口参数：无
返回值：  无
说明  ：  周期和分频比决定了定时长短，修改这两个值可修改定时时长
************************************************/
void TIM_Config(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_DeInit(TIM3);
  TIM_TimeBaseStructure.TIM_Period        = 249;	//自动重装载寄存器的值
  TIM_TimeBaseStructure.TIM_Prescaler     = (36 - 1);//时钟预分频数
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;//采样分频
  TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  TIM_ClearFlag(TIM3, TIM_FLAG_Update); //清除溢出中断标志
  TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
  TIM_Cmd(TIM3, ENABLE); //
}

/************************************************
函数名称：void NVIC_Config(void)
函数功能：对中断优先级进行配置
入口参数：无
返回值：  无
说明  ：  这里只有一个中断，配置成哪个都可以
************************************************/
 void NVIC_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  			   
  NVIC_InitStructure.NVIC_IRQChannel                   = TIM3_IRQn; //定时器3通道
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

//
static unsigned char GetKeyValue(void)
{
	uint8_t key_value = 0;
	
  if(KEY1 == 0)
    key_value |= 0x01;     //按键1按下
  if(KEY2 == 0)
    key_value |= 0x02;    //按键2按下
  if(KEY3 == 0)
    key_value |= 0x04;    //按键3按下
	if(KEY4 == 0)
		key_value |= 0x08;    //按键4按下
	
  return key_value;     //没有键按下
}

//
unsigned char KeyScan(void)
{
	
		unsigned char keypressValue = 0x00;    //按键值
		static unsigned char keyreturnvalue;    //按键返回值
		static uint8_t key_cnt=0;
		static unsigned char key_state = 0;  //按键状态
		static unsigned char keyprev = 0;  //上一次按键状态
		static unsigned int keydelay = 0; //按键连发计时
		static unsigned int keylong = 0;  //按键长按计时

		keypressValue = GetKeyValue();  //得到按键值
		switch(key_state)
		{
			case 0:    //按键初始状态

								if( keypressValue != 0x00 )    //有键按下
								{
										key_state = 1;         //转到下一个状态，确认按键
										keyprev ^= keypressValue;  //保存按键状态
								}
					break;
			case 1://按键确认状态

								if( keypressValue ^ keyprev ) //确认和上次按键相同
								{
										key_cnt++;
										if( key_cnt > 3 )//判断多次
										{
												key_cnt = 0;										
												key_state = 2;  //转到下一个状态，判断长按等
												keyreturnvalue = KEY_DOWN | keypressValue;    //按键确认被按下，即按键按下就响应，不等按键抬起
												keyprev = keypressValue;
										}
								}
								else     //本次按键和上次按键不相同，为抖动，返回到状态0
								{
										key_cnt = 0;
										key_state = 0;
								}
					break;
		 case 2://按键释放或者长按
								
								if( keypressValue ^ keyprev )   //按键释放
								{ 
										key_state = 0;
										keydelay = 0;
										keylong = 0;
										keyreturnvalue = KEY_UP | keyprev;   //按键抬起后才返回按键值
										break;
								}
//								if( keypressValue == keyprev )
//								{
//										keydelay++;
//										if( (keylong == 1) && (keydelay > KEY_SERIES_DELAY) )
//										{ 
//												keydelay = 0;
//												keyreturnvalue = KEY_LIAN | keyprev;
//												keyprev = keypressValue;  //记住上次的按键
//												break;
//										}
//										if(keydelay > KEY_LONG_FLAG)
//										{
//												keylong = 1;
//												keydelay = 0;
//												keyreturnvalue = KEY_LONG | keyprev;    //返回长按后的值
//												break;
//										}
//								}
			default:
			break;
		}
		return keyreturnvalue;
}

//
void KeyProcess(unsigned char KeyValue)
{
		if( KeyValue & KEY_DOWN )
		{
		    switch(KeyValue)
				{
					case 0xA1:
						GPIO_ResetBits(GPIOF,GPIO_Pin_6);	//点亮LED灯
					break;
					case 0xA2:  
						GPIO_ResetBits(GPIOF,GPIO_Pin_7);	//点亮LED灯
					break;
					case 0xA4:
						GPIO_ResetBits(GPIOF,GPIO_Pin_8);	//点亮LED灯
					break;
					case 0xA8:
						GPIO_ResetBits(GPIOF,GPIO_Pin_9);	//点亮LED灯
					break;
					
					default:
						break;
				}
		}
		if( KeyValue & KEY_UP )
		{
		    switch(KeyValue)
				{
					case 0xB1:
						GPIO_SetBits(GPIOF,GPIO_Pin_6);	//熄灭LED灯
					break;
					case 0xB2:
						GPIO_SetBits(GPIOF,GPIO_Pin_7);	//熄灭LED灯
					break;
					case 0xB4:
						GPIO_SetBits(GPIOF,GPIO_Pin_8);	//熄灭LED灯
					break;
					case 0xB8:
						GPIO_SetBits(GPIOF,GPIO_Pin_9);	//熄灭LED灯
					break;
					
					default:
						break;
				}
		}
}

//
void KEY (void)
{
		 GetKeyValue();
		 KeyValue = KeyScan();
		 KeyProcess(KeyValue) ;
}


