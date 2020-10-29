/************************************************
文件创建时间：2014-7-19
文件版本：    V1
版本作者：    中山火炬职业技术学院电子信息工程专业    杨立宏
文件实现功能： 用通用定时器TIM3实现一个LED灯的闪烁
************************************************/
#include "stm32f10x.h"	   //系统文件，必须要包含
#include "user.h"

u32 cnt2 = 0;


/*************主函数**************************/
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
//					  GPIO_WriteBit(GPIOF, GPIO_Pin_6, led_status);	//点亮LED灯
				}
				KEY();	
		}
}
