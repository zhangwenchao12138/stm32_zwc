#include "sysinc.h"


 int main(void)
 {	
	//u32 tep=10000;
	NVIC_init();
	SYSTICK_DelayInit();	    	 //延时函数初始化	  	  	
	GPIO_QuickInit(HW_GPIOC, GPIO_Pin_13, GPIO_Mode_Out_PP);//初始化与LED连接的硬件接口
	GPIO_QuickInit(HW_GPIOB, GPIO_Pin_9, GPIO_Mode_Out_PP);//初始化与LED连接的硬件接口
	UART_QuickInit(HW_UART1, 9600, 2, 2, ENABLE);
	EXTI_QuickInit(HW_EXTIA, EXTI_Pin_0, 3, 3);
  TIM_PWM_Init(TIM2, PWM_CH1, 1000, ENABLE);
  TIM_PWM_Init(TIM2, PWM_CH2, 1000, ENABLE);
  TIM_PWM_Init(TIM2, PWM_CH3, 1000, ENABLE);
  TIM_PWM_Init(TIM2, PWM_CH4, 1000, ENABLE);
	TIM_SetCompare(TIM2, PWM_CH1, 8000);
	TIM_SetCompare(TIM2, PWM_CH2, 1300);
	//TIM_SetCompare(TIM2, PWM_CH3, 10000);
	//TIM_SetCompare(TIM2, PWM_CH4, 10000);
	//PBout(9)=1;
	while(1)
	{		
	//TIM_SetCompare(TIM2, PWM_CH1, tep);
	//TIM_SetCompare(TIM2, PWM_CH2, tep);
	//TIM_SetCompare(TIM2, PWM_CH3, tep);
	//TIM_SetCompare(TIM2, PWM_CH4, tep);
		//if(tep >= 1000)
		//tep = tep-1000;
		//else   tep =10000;
		PCout(13)=0;
		SYSTICK_DelayMs(200);	 //延时300ms
		PCout(13)=1;	
		SYSTICK_DelayMs(200);	//延时300ms
	}
}

