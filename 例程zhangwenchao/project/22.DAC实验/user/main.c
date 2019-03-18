
#include "sysinc.h"

void timer2()
{	
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
		{ PBout(9)= !PBout(9);
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  //清除TIMx更新中断标志 
		}
}

int main()
{
	NVIC_init();
 	SYSTICK_DelayInit();
	Chip_0832_Port_Init();
	GPIO_QuickInit(HW_GPIOC, GPIO_Pin_13, GPIO_Mode_Out_PP); 
	TIM_us_Init(TIM2, 30000, TIM_IT_Update,2, ENABLE);	
 	TIMER_CallbackInstall(HW_TIMER2, timer2);
	
	Write_0832_Date(0xff);
	while(1)
	{
		SYSTICK_DelayMs(1000);	
		PCout(13) = ~PCout(13);
		Write_0832_V(1.5);
	}
}






