#include "stm32f10x.h"
#include "user.h"


unsigned char flag=0;
unsigned char KeyValue=0;
uchar buff[3];



/************************************************
�������ƣ�void RCC_Config(void)
�������ܣ����õ���ʱ�ӽ��г�ʼ�����򿪸�ģ��ʱ��
��ڲ�������
����ֵ��  ��
˵��  ��  ��Ҫ�Ǵ�GPIOF��TIM3��ʱ��
************************************************/
void RCC_Config(void)
{
	
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOA
	                        | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD
	                        | RCC_APB2Periph_GPIOB,ENABLE);	//GPIOFʱ�Ӵ�
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);	//TIM3ʱ�Ӵ�
	
}

/************************************************
�������ƣ�void GPIO_Config(void)
�������ܣ���LED�ƶ˿ڽ��г�ʼ��
��ڲ�������
����ֵ��  ��
˵��  ��  �õ���PF6�ܽţ�����Ϊ�������
************************************************/
void GPIO_Config(void)
{
	
  GPIO_InitTypeDef GPIO_InitStructure;
	//key
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0| GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	  //��������
  GPIO_Init(GPIOA,&GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	  //��������
  GPIO_Init(GPIOC,&GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	  //��������
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
�������ƣ�void TIM_Config(void)
�������ܣ���TIM3���г�ʼ��
��ڲ�������
����ֵ��  ��
˵��  ��  ���ںͷ�Ƶ�Ⱦ����˶�ʱ���̣��޸�������ֵ���޸Ķ�ʱʱ��
************************************************/
void TIM_Config(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_DeInit(TIM3);
  TIM_TimeBaseStructure.TIM_Period        = 249;	//�Զ���װ�ؼĴ�����ֵ
  TIM_TimeBaseStructure.TIM_Prescaler     = (36 - 1);//ʱ��Ԥ��Ƶ��
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;//������Ƶ
  TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  TIM_ClearFlag(TIM3, TIM_FLAG_Update); //�������жϱ�־
  TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
  TIM_Cmd(TIM3, ENABLE); //
}

/************************************************
�������ƣ�void NVIC_Config(void)
�������ܣ����ж����ȼ���������
��ڲ�������
����ֵ��  ��
˵��  ��  ����ֻ��һ���жϣ����ó��ĸ�������
************************************************/
 void NVIC_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  			   
  NVIC_InitStructure.NVIC_IRQChannel                   = TIM3_IRQn; //��ʱ��3ͨ��
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
    key_value |= 0x01;     //����1����
  if(KEY2 == 0)
    key_value |= 0x02;    //����2����
  if(KEY3 == 0)
    key_value |= 0x04;    //����3����
	if(KEY4 == 0)
		key_value |= 0x08;    //����4����
	
  return key_value;     //û�м�����
}

//
unsigned char KeyScan(void)
{
	
		unsigned char keypressValue = 0x00;    //����ֵ
		static unsigned char keyreturnvalue;    //��������ֵ
		static uint8_t key_cnt=0;
		static unsigned char key_state = 0;  //����״̬
		static unsigned char keyprev = 0;  //��һ�ΰ���״̬
		static unsigned int keydelay = 0; //����������ʱ
		static unsigned int keylong = 0;  //����������ʱ

		keypressValue = GetKeyValue();  //�õ�����ֵ
		switch(key_state)
		{
			case 0:    //������ʼ״̬

								if( keypressValue != 0x00 )    //�м�����
								{
										key_state = 1;         //ת����һ��״̬��ȷ�ϰ���
										keyprev ^= keypressValue;  //���水��״̬
								}
					break;
			case 1://����ȷ��״̬

								if( keypressValue ^ keyprev ) //ȷ�Ϻ��ϴΰ�����ͬ
								{
										key_cnt++;
										if( key_cnt > 3 )//�ж϶��
										{
												key_cnt = 0;										
												key_state = 2;  //ת����һ��״̬���жϳ�����
												keyreturnvalue = KEY_DOWN | keypressValue;    //����ȷ�ϱ����£����������¾���Ӧ�����Ȱ���̧��
												keyprev = keypressValue;
										}
								}
								else     //���ΰ������ϴΰ�������ͬ��Ϊ���������ص�״̬0
								{
										key_cnt = 0;
										key_state = 0;
								}
					break;
		 case 2://�����ͷŻ��߳���
								
								if( keypressValue ^ keyprev )   //�����ͷ�
								{ 
										key_state = 0;
										keydelay = 0;
										keylong = 0;
										keyreturnvalue = KEY_UP | keyprev;   //����̧���ŷ��ذ���ֵ
										break;
								}
//								if( keypressValue == keyprev )
//								{
//										keydelay++;
//										if( (keylong == 1) && (keydelay > KEY_SERIES_DELAY) )
//										{ 
//												keydelay = 0;
//												keyreturnvalue = KEY_LIAN | keyprev;
//												keyprev = keypressValue;  //��ס�ϴεİ���
//												break;
//										}
//										if(keydelay > KEY_LONG_FLAG)
//										{
//												keylong = 1;
//												keydelay = 0;
//												keyreturnvalue = KEY_LONG | keyprev;    //���س������ֵ
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
						GPIO_ResetBits(GPIOF,GPIO_Pin_6);	//����LED��
					break;
					case 0xA2:  
						GPIO_ResetBits(GPIOF,GPIO_Pin_7);	//����LED��
					break;
					case 0xA4:
						GPIO_ResetBits(GPIOF,GPIO_Pin_8);	//����LED��
					break;
					case 0xA8:
						GPIO_ResetBits(GPIOF,GPIO_Pin_9);	//����LED��
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
						GPIO_SetBits(GPIOF,GPIO_Pin_6);	//Ϩ��LED��
					break;
					case 0xB2:
						GPIO_SetBits(GPIOF,GPIO_Pin_7);	//Ϩ��LED��
					break;
					case 0xB4:
						GPIO_SetBits(GPIOF,GPIO_Pin_8);	//Ϩ��LED��
					break;
					case 0xB8:
						GPIO_SetBits(GPIOF,GPIO_Pin_9);	//Ϩ��LED��
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


