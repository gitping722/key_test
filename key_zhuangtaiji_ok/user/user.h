#ifndef __USER_H
#define __USER_H


#define uchar  unsigned char
#define uint   unsigned int
	
#define  KEY1     GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)        //读取引脚输入
#define  KEY2     GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13)    
#define  KEY3     GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8)    
#define  KEY4     GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_3)
#define  KEY_LONG_FLAG  200
#define  KEY_SERIES_DELAY  20
#define  KEY_DOWN     0xA0                                   //按下亮
#define  KEY_UP       0xB0                                   //按下抬起亮
#define  KEY_LONG     0xC0
#define  KEY_LIAN     0xD0



void RCC_Config(void);
void GPIO_Config(void);
void TIM_Config(void);
void NVIC_Config(void);
void KEY(void);
void k2_main(void);
void k1_main(void);
unsigned char KeyScan(void);
static unsigned char GetKeyValue(void);


#endif
