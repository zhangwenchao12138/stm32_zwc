#include "sysinc.h"

u8 txbuf[33]={1,7,3,4};
u8 rxbuf[33]={1,9,3,4};

 int main(void)
 {	
	u32 tep=10000;
	NVIC_init();
	SYSTICK_DelayInit();	    	 //延时函数初始化	  	  	
	GPIO_QuickInit(HW_GPIOC, GPIO_Pin_13, GPIO_Mode_Out_PP);//初始化与LED连接的硬件接口
	GPIO_QuickInit(HW_GPIOB, GPIO_Pin_9, GPIO_Mode_Out_PP);//初始化与LED连接的硬件接口
	GPIO_QuickInit(HW_GPIOB, GPIO_Pin_10, GPIO_Mode_Out_PP);//初始化与LED连接的硬件接口
	GPIO_QuickInit(HW_GPIOB, GPIO_Pin_11, GPIO_Mode_Out_PP);//初始化与LED连接的硬件接口
	UART_QuickInit(HW_UART1, 9600, 2, 2, ENABLE);
	EXTI_QuickInit(HW_EXTIA, EXTI_Pin_0, 3, 3);
	//TIM_us_Init(TIM3, 300000, TIM_IT_Update,2, ENABLE);
  TIM_PWM_Init(TIM4, PWM_CH3, 1000, ENABLE);
	TIM_SetCompare(TIM4, PWM_CH3, 10000);
	//NRF24L01_Init(Spics); 
	 PBout(9)=1;
	while(1)
	{		
			TIM_SetCompare(TIM4, PWM_CH3, tep);
		if(tep >= 1000)
		tep = tep-1000;
		else   tep =10000;
		PCout(13)=0;
			 PBout(11)=1;
	 PBout(10)=1;
		SYSTICK_DelayMs(900);	 //延时300ms
		PCout(13)=1;
			 PBout(11)=0;
	 PBout(10)=0;
		
		SYSTICK_DelayMs(900);	//延时300ms
	}
 }

