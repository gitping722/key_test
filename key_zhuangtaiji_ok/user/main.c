/************************************************
�ļ�����ʱ�䣺2014-7-19
�ļ��汾��    V1
�汾���ߣ�    ��ɽ���ְҵ����ѧԺ������Ϣ����רҵ    ������
�ļ�ʵ�ֹ��ܣ� ��ͨ�ö�ʱ��TIM3ʵ��һ��LED�Ƶ���˸
************************************************/
#include "stm32f10x.h"	   //ϵͳ�ļ�������Ҫ����
#include "user.h"

u32 cnt2 = 0;


/*************������**************************/
int main(void)
{		
	
//	  uint8_t led_status = 0;
	
		RCC_Config();
		GPIO_Config();
		TIM_Config();
		NVIC_Config();
	
		while(1)
		{
			
				if(cnt2==4000)
				{
						cnt2 = 0;
//					  led_status = !led_status;
//					  GPIO_WriteBit(GPIOF, GPIO_Pin_6, led_status);	//����LED��
				}
				KEY();	
		}
}
